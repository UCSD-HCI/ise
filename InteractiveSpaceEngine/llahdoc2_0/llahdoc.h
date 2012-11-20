#ifndef _LLAH_DOC_H
#define _LLAH_DOC_H

#include <opencv2/core/core.hpp>

#ifndef	__STRDISC__
#define	__STRDISC__
// 連続値のアフィン不変量を離散値に変換するための構造体
typedef struct _strDisc {
	double min;	// 最小値
	double max;	// 最大値
	int num;	// 離散化レベル
	int res;	// datの個数
	int *dat;	// 離散値を入れる配列
} strDisc;
#endif

#ifndef	__STRHIST__
#define	__STRHIST__
//アフィン不変量のヒストグラム構築用の構造体
typedef struct _strHist {
	int size;	// binの数
	int *bin;	// データ
	double min;	// データの最小値
	double max;	// データの最大値．minとmaxで入るbinを決める
} strHist;
#endif

#ifndef	__STRHASH__
#define	__STRHASH__
typedef unsigned char strHash; // 配列でハッシュ構成
#endif

typedef struct _strLlahDocDb {
	strDisc disc;
	strHash **hash;
	strHash *storage;
	CvPoint **reg_pss;
	CvSize *reg_sizes;
	double **reg_areass;
	int *reg_nums;
	char **dbcors;
} strLlahDocDb;

int LlahDocConstructDb( int n, int m, int d, const char *reg_dir, const char *reg_suffix, const char *db_dir );
strLlahDocDb *LlahDocLoadDb( const char *db_dir );
void LlahDocReleaseDb( strLlahDocDb *db );
int LlahDocRetrieveImage( const char *img_fname, strLlahDocDb *db, char *result, int result_len );
int LlahDocRetrieveIplImage( IplImage *img, strLlahDocDb *db, char *result, int result_len );
#endif