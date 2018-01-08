/*
###################################################################################
#
# Polylib - Polygon Management Library
#
# Copyright (c) 2010-2011 VCAD System Research Program, RIKEN.
# All rights reserved.
#
# Copyright (c) 2012-2015 Advanced Institute for Computational Science (AICS), RIKEN.
# All rights reserved.
#
# Copyright (c) 2016-2018 Research Institute for Information Technology (RIIT), Kyushu University.
# All rights reserved.
#
###################################################################################
*/

#ifndef polylib_common_h
#define polylib_common_h

#include "common/PolylibDefine.h"


#ifdef WIN32
#define __func__ __FUNCTION__
#define PL_DEBGOSH std::cout

#else

#define PL_DEBGOSH std::cout
#ifndef __func__
#define __func__ __FILE__
#endif

#endif




#include <iostream>

namespace PolylibNS {

	////////////////////////////////////////////////////////////////////////////
	///
	/// 三角形IDファイルフォーマット
	///
	////////////////////////////////////////////////////////////////////////////
	typedef enum {
		ID_BIN,		///< バイナリ形式で入出力を行う。
		ID_ASCII	///< アスキー形式で入出力を行う。
	} ID_FORMAT;

	////////////////////////////////////////////////////////////////////////////
	///
	/// デバッグ出力先、エラー時出力先
	///
	////////////////////////////////////////////////////////////////////////////
#define PL_DBGOS    std::cout
#define PL_DBGOSH   std::cout<<gs_rankno<<"PL:"
#define PL_ERROS    std::cerr
#define PL_ERROSH   std::cerr<<gs_rankno<<"PL:"

	////////////////////////////////////////////////////////////////////////////
	///
	/// デバッグ出力用ランク番号グローバル文字列
	///
	////////////////////////////////////////////////////////////////////////////
	extern std::string gs_rankno;

} // end of namespace PolylibNS
#endif // polylib_common_h
