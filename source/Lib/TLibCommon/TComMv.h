/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.
 *
 * Copyright (c) 2010-2017, ITU/ISO/IEC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  * Neither the name of the ITU/ISO/IEC nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/** \file     TComMv.h
    \brief    motion vector class (header)
*/

#ifndef __TCOMMV__
#define __TCOMMV__

#include "CommonDef.h"
#include "hyz.h"
#include "OData.h"
extern OData globalOData;
//! \ingroup TLibCommon
//! \{

// ====================================================================================================================
// Class definition
// ====================================================================================================================
//extern OData globalOData;
/// basic motion vector class
class TComMv
{
private:
  Short m_iHor;     ///< horizontal component of motion vector
  Short m_iVer;     ///< vertical component of motion vector

public:

  // ------------------------------------------------------------------------------------------------------------------
  // constructors
  // ------------------------------------------------------------------------------------------------------------------

  TComMv() :
  m_iHor(0),
  m_iVer(0)
  {
  }

  TComMv( Short iHor, Short iVer ) :
  m_iHor(iHor),
  m_iVer(iVer)
  {
  }

  // ------------------------------------------------------------------------------------------------------------------
  // set
  // ------------------------------------------------------------------------------------------------------------------

  Void  set       ( Short iHor, Short iVer)     { m_iHor = iHor;  m_iVer = iVer;            }
  Void  setHor    ( Short i )                   { m_iHor = i;                               }
  Void  setVer    ( Short i )                   { m_iVer = i;                               }
  Void  setZero   ()                            { m_iHor = m_iVer = 0;  }

  // ------------------------------------------------------------------------------------------------------------------
  // get
  // ------------------------------------------------------------------------------------------------------------------

  Int   getHor    () const { return m_iHor;          }
  Int   getVer    () const { return m_iVer;          }
  Int   getAbsHor () const { return abs( m_iHor );   }
  Int   getAbsVer () const { return abs( m_iVer );   }

  // ------------------------------------------------------------------------------------------------------------------
  // operations
  // ------------------------------------------------------------------------------------------------------------------

  const TComMv& operator += (const TComMv& rcMv)
  {
    m_iHor += rcMv.m_iHor;
    m_iVer += rcMv.m_iVer;
    return  *this;
  }

  const TComMv& operator-= (const TComMv& rcMv)
  {
    m_iHor -= rcMv.m_iHor;
    m_iVer -= rcMv.m_iVer;
    return  *this;
  }

#if !ME_ENABLE_ROUNDING_OF_MVS
  const TComMv& operator>>= (const Int i)
  {
    m_iHor >>= i;
    m_iVer >>= i;
    return  *this;
  }
#endif

#if ME_ENABLE_ROUNDING_OF_MVS
  //! shift right with rounding
  Void divideByPowerOf2 (const Int i)
  {
    Int offset = (i == 0) ? 0 : 1 << (i - 1);
    m_iHor += offset;
    m_iVer += offset;

    m_iHor >>= i;
    m_iVer >>= i;
  }
#endif

  const TComMv& operator<<= (const Int i)
  {
    m_iHor <<= i;
    m_iVer <<= i;
    return  *this;
  }

  const TComMv operator - ( const TComMv& rcMv ) const
  {
    return TComMv( m_iHor - rcMv.m_iHor, m_iVer - rcMv.m_iVer );
  }

  const TComMv operator + ( const TComMv& rcMv ) const
  {
    return TComMv( m_iHor + rcMv.m_iHor, m_iVer + rcMv.m_iVer );
  }

  Bool operator== ( const TComMv& rcMv ) const
  {
    return (m_iHor==rcMv.m_iHor && m_iVer==rcMv.m_iVer);
  }

  Bool operator!= ( const TComMv& rcMv ) const
  {
    return (m_iHor!=rcMv.m_iHor || m_iVer!=rcMv.m_iVer);
  }

  
#if HYZ_OF_FRAME
#if HYZ_OF_ONE_DIR
  const TComMv scaleMv(Int iScale) const
  {
	  Int mvx = iScale * getHor();
	  Int mvy = iScale * getVer();
	  return TComMv( mvx, mvy );
  }
#else
  const TComMv scaleMv(Float iScaleX, Float iScaleY, Float ori) const
  {
	  /*Int mvx = Clip3(-32768, 32767, (iScaleX * getHor() + 127 + (iScaleX * getHor() < 0)) >> 8);
	  Int mvy = Clip3(-32768, 32767, (iScaleY * getVer() + 127 + (iScaleY * getVer() < 0)) >> 8);*/
	  Int mvx = Int(Float(iScaleX) * Float(getHor()));
	  Int mvy = Int(Float(iScaleY) * Float(getVer()));
	  Int iScale = (Int)ori;
	  Int mvx2 = Clip3(-32768, 32767, (iScale * getHor() + 127 + (iScale * getHor() < 0)) >> 8);
	  Int mvy2 = Clip3(-32768, 32767, (iScale * getVer() + 127 + (iScale * getVer() < 0)) >> 8);
	  if (iScale == 4096)
	  {
		  mvx2 = getHor();
		  mvy2 = getVer();
	  }
	  
	  //std::cout << mvx << " VS: " << mvx2 << "  " << mvy << " VS: " << mvy2 << std::endl;
	  return TComMv(mvx, mvy);
  }
#endif
#elif HYZ_OF_CTU
  const TComMv scaleMv_ori(Int iScale) const
  {
	  Int mvx = Clip3(-32768, 32767, (iScale * getHor() + 127 + (iScale * getHor() < 0)) >> 8);
	  Int mvy = Clip3(-32768, 32767, (iScale * getVer() + 127 + (iScale * getVer() < 0)) >> 8);

	  return TComMv(mvx, mvy);
  }
  //const TComMv scaleMv(Float hm, Float x, Float y, Int iScale) const // I decide to avoiding using this function for its unclear meaning
  //{
	 // Int mvx = Int((Float)getHor() * x);
	 // Int mvy = Int((Float)getVer() * y);
	 // mvx = Clip3(-32768, 32767, (iScale * getHor() + 127 + (iScale * getHor() < 0)) >> 8);
	 // mvy = Clip3(-32768, 32767, (iScale * getVer() + 127 + (iScale * getVer() < 0)) >> 8);
	 // 
	 // return TComMv(mvx, mvy);
  //}
 /* const TComMv scaleMv(Float hm, Float x, Float y) const // I decide to avoiding using this function for its unclear meaning
  {
	  Int mvx = getHor();
	  Int mvy = getVer();
	  mvx = Int((Float)mvx * x);
	  mvy = Int((Float)mvy * y);
	  return TComMv(mvx, mvy);
  }*/
  const TComMv scaleMv_deep(Float hm, Float x, Float y) const// We strongly recommend to use this function!
  {
	  Int mvx = getHor();
	  Int mvy = getVer();
	  mvx = Int((Float)mvx * x);
	  mvy = Int((Float)mvy * y);
	  return TComMv(mvx, mvy);
  }
  const TComMv scaleMv_deep(Float x, Float y) const// We strongly recommend to use this function!
  {
	  Int mvx = getHor();
	  Int mvy = getVer();
	  mvx = Int((Float)mvx * x);
	  mvy = Int((Float)mvy * y);
	  return TComMv(mvx, mvy);
  }
#else
  const TComMv scaleMv(Int iScale) const
  {
	  Int mvx = Clip3(-32768, 32767, (iScale * getHor() + 127 + (iScale * getHor() < 0)) >> 8);
	  Int mvy = Clip3(-32768, 32767, (iScale * getVer() + 127 + (iScale * getVer() < 0)) >> 8);
	  return TComMv(mvx, mvy);
  }
#endif
};// END CLASS DEFINITION TComMV

//! \}

#endif // __TCOMMV__
