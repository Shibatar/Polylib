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


#ifndef MPIPolylib_h
#define MPIPolylib_h

#include "mpi.h"
#include <exception>
#include <typeinfo>
#include <vector>
#include <map>
#include "Polylib.h"
#include "groups/PolygonGroup.h"
#include "polygons/DVertexTriangle.h"
#include "polygons/DVertex.h"
#include "file_io/TriMeshIO.h"

// MPI通信用メッセージタグ
#define	MPITAG_NUM_CONFIG			1
#define	MPITAG_CONFIG				2
#define	MPITAG_NUM_TRIAS			3
#define MPITAG_TRIA_IDS				4
#define MPITAG_TRIA_EXIDS			5
#define MPITAG_TRIAS				6
#define MPITAG_TRIA_NDATA			7
#define MPITAG_TRIA_SCALAR			8
#define MPITAG_TRIA_VECTOR			9

//#define PL_MPI_REAL MPI_DOUBLE
#ifdef PL_REAL_FLOAT
#define PL_MPI_REAL MPI_FLOAT
#else
#define PL_MPI_REAL MPI_DOUBLE
#endif

namespace PolylibNS {
////////////////////////////////////////////////////////////////////////////
///
/// クラス:ParallelInfo
/// 並列プロセス情報。
///
////////////////////////////////////////////////////////////////////////////

struct ParallelInfo {
	/// MPIコミュニケータ
	MPI_Comm m_comm;

	/// ランク数
	int m_rank;

	/// 計算領域情報
	CalcAreaInfo m_area;

	/// migrate除外三角形IDマップ(k:グループID, v:三角形IDリスト)
	std::map< int, std::vector<int> > m_exclusion_map;
};


////////////////////////////////////////////////////////////////////////////
///
/// クラス:MPIPolylib
/// ポリゴンを管理する為の並列版クラスライブラリです。
///
////////////////////////////////////////////////////////////////////////////
class MPIPolylib : public Polylib {
public:
	///
	/// インスタンス取得。本クラスはsingltonクラスです。
	///
	/// @return MPIPolylibクラスのインスタンス
	///
	static MPIPolylib* get_instance();

	///
	/// 並列計算関連情報の設定と初期化を行う。
	/// 全rankで各々設定を行い、その領域情報を全rankへ配信する。
	///
	///  @param[in] comm	MPIコミュニケーター
	///  @param[in] bpos	自PE担当領域の基点座標
	///  @param[in] bbsize	同、計算領域のボクセル数
	///  @param[in] gcsize	同、ガイドセルのボクセル数
	///  @param[in] dx		同、ボクセル１辺の長さ
	///  @return POLYLIB_STATで定義される値が返る。
	///
	POLYLIB_STAT
		init_parallel_info(
		MPI_Comm comm,
		PL_REAL bpos[3],
		unsigned int bbsize[3],
		unsigned int gcsize[3],
		PL_REAL dx[3]
	);

	///
	/// Polylib::load()のオーバライドメソッド。
	/// @attention 並列環境では利用できません。
	///
	/// @param[in] config_filename	初期化ファイル名。
	/// @return 常に PLSTAT_NG が返ります。
	///
	POLYLIB_STAT
		load(
		std::string config_filename
		);

	///
	/// rank0によるデータ構築。
	/// 指定された設定ファイルをrank0にて読み込み、グループ階層構造の構築
	/// およびポリゴンデータの構築を行う。
	/// グループ階層構造は全rankにb_castされ、情報を共有する。
	/// ポリゴンデータは各rank領域毎のデータが分配される。
	///
	/// @param[in] config_filename	初期化ファイル名。未指定時はデフォルトファイルを読む。
	/// @return	POLYLIB_STATで定義される値が返る。
	///
	POLYLIB_STAT
		load_rank0(
		std::string config_filename = "",
		PL_REAL scale = 1.0
		);

	///
	/// rank0によるデータ読み込みとデータ構築のみ行う
	/// 指定された設定ファイルをrank0にて読み込み、グループ階層構造の構築
	/// およびポリゴンデータの構築を行う。
	/// グループ階層構造およびポリゴンデータの各rank分配は行わない。
	///
	/// @param[in] config_filename	初期化ファイル名。未指定時はデフォルトファイルを読む。
	/// @return	POLYLIB_STATで定義される値が返る。
	///
	POLYLIB_STAT
		load_only_in_rank0(
		std::string config_filename = "",
		PL_REAL scale = 1.0
		);

	///
	/// load_only_in_rank0()により構築されたポリゴンデータを
	/// 各rankへの分配のみ行う。
	///
	///
	POLYLIB_STAT
		distribute_only_from_rank0(
		);

	///
	/// 全rank並列でのデータ構築。
	/// 指定された設定ファイルを各rankにて読み込み、グループ階層構造の構築、
	/// およびポリゴンデータの構築を行う。
	/// @attention 各rankが読み込むファイルに記述されたグループ階層構造が一致している必要がある。
	///
	/// @param[in] config_filename	初期化ファイル名。未指定時はデフォルトファイルを読む。
	/// @param[in] id_format		三角形IDファイルの入力形式。
	/// @return	POLYLIB_STATで定義される値が返る。
	///
	POLYLIB_STAT
		load_parallel(
		std::string config_filename = "",
		ID_FORMAT	id_format = ID_BIN
		);

	///
	/// Polylib::save()のオーバライドメソッド。
	/// @attention 並列環境では利用できません。
	///
	/// @param[out] p_config_filename	初期化ファイル名。
	/// @return 常に PLSTAT_NG が返ります。
	///
	POLYLIB_STAT
		save(
		std::string *p_config_filename
		);

	///
	/// rank0によるデータ保存。
	/// rank0の本クラスインスタンスが保持するグループ階層構造を設定ファイルに書き出す。
	/// 同時に各rankに分散するポリゴンデータもrank0に集められ、指定されたフォーマットの
	/// STL/OBJファイルにrank0で書き出す。
	/// 設定ファイル命名規則は以下の通り
	///   polylib_config_付加文字列.tpp
	/// STL/OBJファイル命名規則は以下の通り
	///   ポリゴングループ名称_付加文字列.拡張子
	///
	/// @param[out] p_config_filename	設定ファイル名返却用stringインスタンスへのポインタ
	/// @param[in]  stl_format STL/OBJファイルフォーマット。 "stl_a":アスキー形式　"stl_b":バイナリ形式 "obj_a":アスキー形式　"obj_b","obj_bb":バイナリ形式,"obj_bb"は、頂点法線付き。
	/// @param[in]  extend				ファイル名に付加する文字列。省略可。省略
	///									した場合は、付加文字列として本メソッド呼
	///									び出し時の年月日時分秒(YYYYMMDD24hhmmss)
	///									を用いる。
	/// @return	POLYLIB_STATで定義される値が返る。
	/// @attention 出力引数p_config_filenameの返却値はrank0でのみ有効
	///
	POLYLIB_STAT
		save_rank0(
		std::string *p_config_filename,
		std::string stl_format,
		std::string extend = ""
		);

	///
	/// 全rank並列でのデータ保存。
	/// 各rankの本クラスインスタンスが保持するグループ階層構造を設定ファイルに各rank毎に書き出す。
	/// 同時にポリゴンデータも指定されたフォーマットのSTL/OBJファイルに各rank毎に書き出す。
	/// 設定ファイル命名規則は以下の通り
	///   polylib_config_ランク番号_付加文字列.tpp
	/// STL/OBJファイル命名規則は以下の通り
	///   ポリゴングループ名称_ランク番号_付加文字列.拡張子
	///
	/// @param[out] p_config_filename	設定ファイル名返却用stringインスタンスへのポインタ
	/// @param[in] stl_format	STL/OBJファイルフォーマット。 "stl_a":アスキー形式　"stl_b":バイナリ形式 "obj_a":アスキー形式　"obj_b","obj_bb":バイナリ形式,"obj_bb"は、頂点法線付き。
	/// @param[in]  extend				ファイル名に付加する文字列。省略可。省略
	///									した場合は、付加文字列として本メソッド呼
	///									び出し時の年月日時分秒(YYYYMMDD24hhmmss)
	///									を用いる。
	/// @param[in] id_format	三角形IDファイルの出力形式。
	/// @return	POLYLIB_STATで定義される値が返る。
	///
	POLYLIB_STAT
		save_parallel(
		std::string *p_config_filename,
		std::string stl_format,
		std::string extend = "",
		ID_FORMAT	id_format = ID_BIN
		);

	///
	/// ポリゴン座標の移動。
	/// 本クラスインスタンス配下の全PolygonGroupのmoveメソッドが呼び出される。
	///
	/// @param[in] params	移動計算要パラメタセット。
	/// @return	POLYLIB_STATで定義される値が返る。
	///
	POLYLIB_STAT
		move(
		PolylibMoveParams &params
		);

	///
	/// ポリゴンデータのPE間移動。
	/// 本クラスインスタンス配下の全PolygonGroupのポリゴンデータについて、
	/// moveメソッドにより移動した三角形ポリゴン情報を隣接PE間でやり取りする。
	///
	/// @return	POLYLIB_STATで定義される値が返る。
	///
	POLYLIB_STAT
		migrate();

	///
	/// m_myprocの内容をget
	/// @return 自PE領域情報
	///
	ParallelInfo get_myproc();

	///
	/// MPIPolylibが利用中の概算メモリ量を返す
	///
	/// @return 利用中のメモリ量(byte)
	///
	unsigned int used_memory_size();

protected:
	///
	/// コンストラクタ。
	/// singletonのため非公開。本クラスインスタンス取得にはget_instance()を利用する。
	///

	MPIPolylib();

	///
	/// デストラクタ。
	///
	~MPIPolylib();

	///
	/// 指定されたグループ以下の階層構造をツリー形式で標準出力に出力する。
	///  @param p	表示対象となるグループのポインタ。
	///  @param tab	階層の深さを示すスペース。
	///  @attention プロセス毎に動作する。
	///   出力にランク数が加わる以外は非並列版と同じ。
	///
	void show_group_name(PolygonGroup* p, std::string tab);

	///
	/// 設定ファイル内容を他rankへbroadcastする。
	///
	/// @param[in] config_contents 初期化ファイル内容。
	/// @return	POLYLIB_STATで定義される値が返る。
	///
	POLYLIB_STAT
		broadcast_config(
		std::string config_contents
		);

	///
	/// 各PE領域内ポリゴン情報を全rankに送信
	///
	/// @return	POLYLIB_STATで定義される値が返る。
	///
	POLYLIB_STAT
		send_polygons_to_all();

	///
	/// グループID＆グループ内三角形数の送信情報を作成。
	///
	/// @param[in,out] p_vec 情報追加先ベクタ
	/// @param[in] group_id グループID
	/// @param[in] p_trias グループ内三角形リスト
	/// @return	POLYLIB_STATで定義される値が返る。
	///
	POLYLIB_STAT
		pack_num_trias(
		std::vector<int>* p_vec,
		int group_id,
		const std::vector<PrivateTriangle*>* p_trias
		);

	///
	/// 三角形の送信情報を作成。
	///
	/// @param[in,out] p_vec 情報追加先ベクタ
	/// @param[in] p_trias グループ内三角形リスト
	/// @return	POLYLIB_STATで定義される値が返る。
	///
	POLYLIB_STAT
		pack_trias(
		std::vector<PL_REAL>* p_vec,
		const std::vector<PrivateTriangle*>* p_trias
		);

	///
	/// DVertex三角形のデータ数を作成
	///
	/// @param[in,out] p_vec 情報追加先ベクタ
	/// @param[in] p_trias グループ内三角形リスト
	/// @return	POLYLIB_STATで定義される値が返る。
	///
	POLYLIB_STAT
		pack_tria_ndata(
		std::vector<int>* p_vec,
		const std::vector<PrivateTriangle*>* p_trias
		);

	///
	/// DVertex三角形のスカラーデータ送信情報を作成。
	///
	/// @param[in,out] p_vec 情報追加先ベクタ
	/// @param[in] p_trias グループ内三角形リスト
	/// @return	POLYLIB_STATで定義される値が返る。
	///
	POLYLIB_STAT
		pack_tria_scalar_data(
		std::vector<PL_REAL>* p_vec,
		const std::vector<PrivateTriangle*>* p_trias
		);

	///
	/// DVertex三角形のベクターデータ送信情報を作成。
	///
	/// @param[in,out] p_vec 情報追加先ベクタ
	/// @param[in] p_trias グループ内三角形リスト
	/// @return	POLYLIB_STATで定義される値が返る。
	///
	POLYLIB_STAT
		pack_tria_vector_data(
		std::vector<PL_REAL>* p_vec,
		const std::vector<PrivateTriangle*>* p_trias
		);


	///
	/// 三角形IDの送信情報を作成。
	///
	/// @param[in,out] p_vec 情報追加先ベクタ
	/// @param[in] p_trias グループ内三角形リスト
	/// @return	POLYLIB_STATで定義される値が返る。
	///
	POLYLIB_STAT
		pack_tria_ids(
		std::vector<int>* p_vec,
		const std::vector<PrivateTriangle*>* p_trias
		);

	///
	/// 三角形のユーザ定義IDの送信情報を作成。
	///
	/// @param[in,out] p_vec 情報追加先ベクタ
	/// @param[in] p_trias グループ内三角形リスト
	/// @return  POLYLIB_STATで定義される値が返る。
	///
	POLYLIB_STAT
		pack_tria_exids(
		std::vector<int>* p_vec,
		const std::vector<PrivateTriangle*>* p_trias
		);

	///
	/// 自領域内ポリゴンのみ抽出してポリゴン情報を再構築。
	/// migrate実行後に行う。
	///
	/// @return	POLYLIB_STATで定義される値が返る。
	///
	POLYLIB_STAT
		erase_outbounded_polygons();

	///
	/// ポリゴングループ定義情報をrank0から受信し、グループ階層構造を構築。
	///
	/// @return	POLYLIB_STATで定義される値が返る。
	///
	POLYLIB_STAT
		broadcast_config_from_rank0();

	///
	/// 自領域に必要なポリゴン情報をrank0から受信
	///
	/// @return	POLYLIB_STATで定義される値が返る。
	///
	POLYLIB_STAT
		receive_polygons_from_rank0();

	///
	/// 他rankからポリゴン情報をrank0で受信
	///
	POLYLIB_STAT
		gather_polygons();

	///
	/// rank0へポリゴン情報を送信
	///
	POLYLIB_STAT
		send_polygons_to_rank0();


	///
	/// 他rankからポリゴン情報をrank0で受信(vtk)
	///
	POLYLIB_STAT
		gather_polygons_vtk();

	///
	/// rank0へポリゴン情報を送信(vtk)
	///
	POLYLIB_STAT
		send_polygons_to_rank0_vtk();


	///
	/// 移動除外三角形IDリストの作成
	///
	POLYLIB_STAT
		select_excluded_trias( PolygonGroup *p_pg );

protected:
	///
	/// プロセス担当領域クラスのポインタを返す
	///  @param[in] rank ランク数
	///  @return プロセス担当領域クラスのポインタ
	///
	ParallelInfo* get_proc(int rank);

	/// 自PE担当領域情報
	ParallelInfo m_myproc;

	/// 自PEを除く全PE担当領域情報リスト
	std::vector<ParallelInfo*> m_other_procs;

	/// 隣接PE担当領域情報リスト
	std::vector<ParallelInfo*> m_neibour_procs;

	/// 自プロセスのランク数
	int m_myrank;

	/// 全プロセス数
	int m_numproc;

	/// 自プロセスが利用するコミュニケーター
	MPI_Comm m_mycomm;
};


} //namespace PolylibNS

#endif // MPIPolylib_h
