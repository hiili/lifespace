/*
 * Copyright (C) 2004-2005 Paul J. Wagner
 * This file is part of the Lifespace Simulator.
 * 
 * Lifespace Simulator is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * Lifespace Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with the Lifespace Simulator; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * For more information about the program:
 *   http://www.cis.hut.fi/pwagner/lifespace/
 */

/**
 * @file Matrix.hpp
 *
 * Lifespace Matrices.
 */

/**
 * @class lifespace::Matrix
 * @ingroup Structures
 *
 * @brief
 * Lifespace matrices.
 *
 * @todo
 * dynamic dimensionality.
 */
#ifndef LS_S_MATRIX_HPP
#define LS_S_MATRIX_HPP


#include "../types.hpp"
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/matrix_expression.hpp>
#include <boost/numeric/ublas/io.hpp>


namespace lifespace {
  
  
  
  
  typedef ublas::matrix<real> Matrix;

  typedef ublas::identity_matrix<real> IdentityMatrix;
  
  
  
  
}   /* namespace lifespace */


#endif   /* LS_S_MATRIX_HPP */
