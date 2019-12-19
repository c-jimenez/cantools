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

#ifndef SAMPLEFUNCTIONS_H
#define SAMPLEFUNCTIONS_H



/** \brief Returns the value of PI */
double SAMPLE_FUNCTION_PiValue(void);

/** \brief Convert an angle value from deg to rad */
double SAMPLE_FUNCTION_ToRad(double deg_value);

/** \brief Convert an angle value from rad to deg */
double SAMPLE_FUNCTION_ToDeg(double rad_value);

/** \brief Convert a speed value from m/s to km/h */
double SAMPLE_FUNCTION_MsToKmh(double ms_value);

/** \brief Computes the hypothenuse of a triangle */
double SAMPLE_FUNCTION_Hypothenuse(double a, double b);


#endif // SAMPLEFUNCTIONS_H
