// -*- mode: c++; mode: auto-fill; mode: flyspell-prog; -*-
// Author Antonio Bulgheroni, INFN <mailto:antonio.bulgheroni@gmail.com>
// Version $Id: EUTelEtaFunctionImpl.cc,v 1.6 2007-07-09 10:19:08 bulgheroni Exp $
/*
 *   This source code is part of the Eutelescope package of Marlin.
 *   You are free to use this source files for your own development as
 *   long as it stays in a public research context. You are not
 *   allowed to use it for commercial purpose. You must put this
 *   header with author names in all development based on this file.
 *
 */

// eutelescope includes ".h" 
#include "EUTelEtaFunctionImpl.h"

// lcio includes <.h>
#include <lcio.h>
#include <IMPL/LCGenericObjectImpl.h>

// system includes <>
#include <vector>
#include <algorithm>

using namespace lcio;
using namespace eutelescope;
using namespace std;

EUTelEtaFunctionImpl::EUTelEtaFunctionImpl(int nBin) : IMPL::LCGenericObjectImpl(0,0,2 * nBin) {
  _typeName        = "Eta function";
  _dataDescription = "The first nBin double values are the bin centers, the second nBin doubles values"
    " the corresponding eta function value";
  _isFixedSize     = true;
}

EUTelEtaFunctionImpl::EUTelEtaFunctionImpl(int nBin, std::vector<double > centerVec, std::vector<double > valueVec) : 
  IMPL::LCGenericObjectImpl(0,0,2 * nBin) {
    _typeName        = "Eta function";
    _dataDescription = "The first nBin double values are the bin centers, the second nBin doubles values"
      " the corresponding eta function value";
    _isFixedSize     = true;
   
 
    unsigned int indexCenter;
    unsigned int indexValue ;
   
    for (indexCenter = 0, indexValue = indexCenter + getNDouble() / 2; 
	 indexCenter < centerVec.size(); indexCenter++, indexValue++ ) {
      setDoubleVal(indexCenter, centerVec[indexCenter]);
      setDoubleVal(indexValue,  valueVec[indexCenter]);
    }
      
}

void EUTelEtaFunctionImpl::setBinCenterVector(std::vector<double > center) {

  unsigned int indexCenter;
  for ( indexCenter = 0; indexCenter < center.size(); indexCenter++ ) {
    setDoubleVal( indexCenter, center[indexCenter] );
  }
}

void EUTelEtaFunctionImpl::setEtaValueVector(std::vector<double > value) {

  unsigned int index;
  int shift = getNDouble() / 2;
  for ( index = 0; index < value.size(); index++ ) {
    setDoubleVal( index + shift, value[index] );
  }
}
  

const vector<double > EUTelEtaFunctionImpl::getBinCenterVector() const {

  vector<double > center(_doubleVec.begin(), _doubleVec.begin() + (getNDouble() / 2) );

  return center;

}

const vector<double > EUTelEtaFunctionImpl::getEtaValueVector() const {

  vector<double > value( _doubleVec.begin() + (getNDouble() / 2), _doubleVec.end());
  return value;

}

int EUTelEtaFunctionImpl::getNoOfBin() const {
  return getNDouble() / 2;
}

double EUTelEtaFunctionImpl::getEtaFromCoG(double x) const {

  typedef vector<double >::const_iterator DoubleIter;
  
  DoubleIter cogBegin = getCoGBeginConstIterator();
  DoubleIter cogEnd   = getCoGEndConstIterator();
  DoubleIter etaBegin = getEtaBeginConstIterator();
  DoubleIter etaEnd   = getEtaEndConstIterator();

  // before doing the binary search, try to see if x isn't at the
  // beginning or at the end of the CoG vector; in such cases the
  // research is useless and the eta value can be returned
  // immediately. Remember that the end() iterator is one element
  // after the last element

  if ( x <= (*cogBegin) )     return (*etaBegin); 
  if ( x >= (*(cogEnd - 1)) ) return (*(etaEnd - 1));
  
  DoubleIter xLeft    = lower_bound(cogBegin, cogEnd, x);
  DoubleIter xRight   = xLeft + 1;
  DoubleIter etaLeft  = etaBegin +  ( xLeft - cogBegin);
  DoubleIter etaRight = etaLeft + 1;

  return *etaLeft + ( *etaLeft - *etaRight ) / ( *xLeft - *xRight ) * ( x - *xLeft) ;

}

vector<double >::const_iterator EUTelEtaFunctionImpl::getCoGBeginConstIterator() const {
  return _doubleVec.begin();
}

vector<double >::const_iterator EUTelEtaFunctionImpl::getCoGEndConstIterator() const {
  return _doubleVec.begin() + (getNDouble() / 2 );
}

vector<double >::const_iterator EUTelEtaFunctionImpl::getEtaBeginConstIterator() const {
  return _doubleVec.begin() + (getNDouble() / 2);
} 

vector<double >::const_iterator EUTelEtaFunctionImpl::getEtaEndConstIterator() const {
  return _doubleVec.end();
} 
