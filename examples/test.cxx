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

#include <string>
#include <iostream>
#include "Polylib.h"

using namespace std;
using namespace PolylibNS;


#ifdef WIN32
int main_test(){
#else
int main(int argc, char** argv ){
#endif


	cout<< "test 0" << endl;

	//Polylib<double>* pl_instance = Polylib<double>::get_instance();
	Polylib* pl_instance = Polylib::get_instance();

	cout<< "test 1" << endl;

	//  pl_instance->load();

	PL_REAL scale=1.0;
	std::string file="config/polylib_config.tp";
	cout<< "test 1 - 1" << endl;
	pl_instance->load(file,scale);

	cout<< "test 2" << endl;

	pl_instance->show_group_hierarchy();

	cout<< "test 3" << endl;

	//  pl_instance->show_group_info("car"); // not working??
	string fname="";
	string stl="stl_a";
	string extend="";

	pl_instance->save(&fname,stl,extend);

	cout<< "test 4" << endl;

	return 0;


}
