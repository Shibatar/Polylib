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

#include "groups/PolygonGroupFactory.h"
#include "groups/PolygonGroup.h"

namespace PolylibNS {

///
/// コンストラクタ。
///
PolygonGroupFactory::PolygonGroupFactory()
{
}

///
/// デストラクタ。
///
PolygonGroupFactory::~PolygonGroupFactory()
{
}

///
/// インスタンス作成。
///
///  @param[in]	class_name	作成するクラス名。
///  @return	作成に失敗した場合はNULLが返る。
///
//	virtual PolygonGroup* create_instance(std::string class_name) {
PolygonGroup* PolygonGroupFactory::create_instance(std::string class_name,PL_REAL tolerance)
{

#ifdef DEBUG
	PL_DBGOSH << "PolygonGroup<T>::create_instance class_name "
		<<class_name<<std::endl;
#endif
	if (class_name == PolygonGroup::get_class_name()) {
		//		  return new PolygonGroup;
		return new PolygonGroup(tolerance);
	}
	else {
		return NULL;
	}

}

} //namespace PolylibNS
