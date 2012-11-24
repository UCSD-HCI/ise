#ifndef _LLAH_DOC_H
#define _LLAH_DOC_H

#include <opencv2/core/core.hpp>

#ifndef	__STRDISC__
#define	__STRDISC__
// �A���l�̃A�t�B���s�ϗʂ𗣎U�l�ɕϊ����邽�߂̍\����
typedef struct _strDisc {
	double min;	// �ŏ��l
	double max;	// �ő�l
	int num;	// ���U�����x��
	int res;	// dat�̌�
	int *dat;	// ���U�l������z��
} strDisc;
#endif

#ifndef	__STRHIST__
#define	__STRHIST__
//�A�t�B���s�ϗʂ̃q�X�g�O�����\�z�p�̍\����
typedef struct _strHist {
	int size;	// bin�̐�
	int *bin;	// �f�[�^
	double min;	// �f�[�^�̍ŏ��l
	double max;	// �f�[�^�̍ő�l�Dmin��max�œ���bin�����߂�
} strHist;
#endif

#ifndef	__STRHASH__
#define	__STRHASH__
typedef unsigned char strHash; // �z��Ńn�b�V���\��
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