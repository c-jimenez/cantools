/*
Copyright(c) 2019 Cedric Jimenez

This file is part of cantools.

cantools is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

cantools is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with cantools.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MAILBOX_H
#define MAILBOX_H


#include "NoCopy.h"

#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <unordered_set>


/** \brief Inter-thread mailbox */
template <typename MsgType>
class MailBox : public INoCopy
{
    public:

        /** \brief Constructor */
        MailBox()
        : m_msgs()
        , m_mutex()
        , m_cond_var()
        {}

        /** \brief Destructor */
        virtual ~MailBox()
        {}

        /** \brief Remove all messages from the mailbox */
        void clear(bool cancel_waiters = false)
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            
            while (!m_msgs.empty())
            {
                m_msgs.pop();
            }

            if (cancel_waiters)
            {
                m_cond_var.notify_all();
                for (auto iter = m_cancel_status.begin(); iter != m_cancel_status.end(); ++iter)
                {
                    *(*iter) = true;
                }
            }
        }

        /** \brief Post a message to in the mailbox */
        void post(const MsgType& msg)
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_msgs.push(msg);
            m_cond_var.notify_all();
        }

        /** \brief Wait for a message from the mailbox */
        bool wait(MsgType& msg)
        {
            bool ret = false;
            std::unique_lock<std::mutex> lock(m_mutex);

            bool cancel = false;
            if (m_msgs.size() == 0)
            {
                m_cancel_status.insert(&cancel);

                do
                {
                    m_cond_var.wait(lock);
                } 
                while ((m_msgs.size() == 0) && !cancel);
                
                m_cancel_status.erase(&cancel);
            }
            if (!cancel)
            {
                msg = m_msgs.front();
                m_msgs.pop();
                ret = true;
            }

            return ret;
        }

        /** \brief Wait for a message from the mailbox */
        bool wait(MsgType& msg, const std::chrono::milliseconds& timeout)
        {
            bool ret = false;
            std::unique_lock<std::mutex> lock(m_mutex);

            bool cancel = false;
            if (m_msgs.size() == 0)
            {
                m_cancel_status.insert(&cancel);

                std::cv_status ret = std::cv_status::no_timeout;
                auto tp_timeout = std::chrono::steady_clock::now() + timeout;
                do
                {
                    ret = m_cond_var.wait_until(lock, tp_timeout);
                } 
                while ((m_msgs.size() == 0) && !cancel && (ret != std::cv_status::timeout));

                m_cancel_status.erase(&cancel);
            }
            if (!cancel && (m_msgs.size() != 0))
            {
                msg = m_msgs.front();
                m_msgs.pop();
                ret = true;
            }

            return ret;
        }        


    private:

        /** \brief Messages */
        std::queue<MsgType> m_msgs;

        /** \brief Mutex */
        std::mutex m_mutex;

        /** \brief Condition variable */
        std::condition_variable m_cond_var;

        /** \brief Cancellation status */
        std::unordered_set<bool*> m_cancel_status;
};


#endif // MAILBOX_H
