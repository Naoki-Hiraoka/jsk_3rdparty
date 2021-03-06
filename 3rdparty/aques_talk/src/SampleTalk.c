/**********************************************************************
	SampleTalk.c - AquesTalk2 Linux 規則音声合成 サンプルプログラム

	標準入力から音声記号列を１行読み込み、
	標準出力に音声波形(.wavフォーマット）を出力

	COPYRIGHT (C) 2010 AQUEST CORP.

	使用方法は、readme.txt を参照ください。
	
	2010/01/23	N.Yamazaki	Creation
	2010/07/07	K.Okada         Modified
**********************************************************************/
#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <AquesTalk2.h>

// AquestTalk クラスのヘッダ
void * file_load(const char * file, int * psize);

int main(int ac, char **av)
{
  char speak_str[65536];
  char phont_fname[1024]={"phont/aq_f3a.phont"},
    input_fname[1024]={""}, output_fname[1024]={"output.wav"};
  int opt;

  while ((opt = getopt(ac, av, "p:o:")) != -1) {
    switch (opt) {
    case 'p':
      strncpy(phont_fname,optarg,1024);
      break;
    case 'o':
      strncpy(output_fname,optarg,1024);
      break;
    default:
      fprintf(stderr, "Usage : %s [-p phont_file] [-o output_file] [intput_file]\n", av[0]);
      exit(EXIT_FAILURE);
    }
  }
  if (optind < ac ) {
    strncpy(input_fname,av[optind],1024);
  }
  fprintf(stderr, "Phont file  : %s\n", phont_fname);
  fprintf(stderr, "Input file  : %s\n", input_fname);
  fprintf(stderr, "Output file : %s\n", output_fname);

  // Phont ファイルの読み込み
  int size;
  void *pPhont = file_load(phont_fname, &size); // Phont ファイルをここで指定
  if(pPhont ==0) {
    fprintf(stderr, "could not found phont file %s\n", phont_fname);
    return-2;
  }

  FILE *input_fp = stdin;
  if ((strlen(input_fname)>0)&&((input_fp = fopen(input_fname, "r"))==NULL)) {
      fprintf(stderr, "could not found input file %s\n", input_fname);
      return -2;
  }
  if(fgets(speak_str,65536-1,input_fp)==0) {
    fprintf(stderr, "could not get input string");
  }
  if ( input_fp != stdin ) fclose(input_fp);

  // メモリ上に音声データを生成
  unsigned char *wav = AquesTalk2_Synthe_Utf8(speak_str, 100, &size, pPhont);
  if(wav==0) {
    fprintf(stderr, "ERR %d\n", size); // エラー時は size にエラーコードが返る
    switch (size) {
      case 100: fprintf(stderr, "その他のエラー\n"); break;
      case 101: fprintf(stderr, "メモリ不足\n"); break;
      case 102: fprintf(stderr, "音声記号列に未定義の読み記号が指定された\n"); break;
      case 103: fprintf(stderr, "韻律データの時間長がマイナスなっている\n"); break;
      case 104: fprintf(stderr, "内部エラー(未定義の区切りコード検出）\n"); break;
      case 105: fprintf(stderr, "音声記号列に未定義の読み記号が指定された\n"); break;
      case 106: fprintf(stderr, "音声記号列のタグの指定が正しくない\n"); break;
      case 107: fprintf(stderr, "タグの長さが制限を越えている（または[>]がみつからない）\n"); break;
      case 108: fprintf(stderr, "タグ内の値の指定が正しくない\n"); break;
      case 109: fprintf(stderr, "WAVE 再生ができない（サウンドドライバ関連の問題）\n"); break;
      case 110: fprintf(stderr, "WAVE 再生ができない（サウンドドライバ関連の問題 非同期再生）\n"); break;
      case 111: fprintf(stderr, "発声すべきデータがない\n"); break;
      case 200: fprintf(stderr, "音声記号列が長すぎる\n"); break;
      case 201: fprintf(stderr, "１つのフレーズ中の読み記号が多すぎる\n"); break;
      case 202: fprintf(stderr, "音声記号列が長い（内部バッファオーバー1）\n"); break;
      case 203: fprintf(stderr, "ヒープメモリ不足\n"); break;
      case 204: fprintf(stderr, "音声記号列が長い（内部バッファオーバー1）\n"); break;
    }
    fprintf(stderr, "Please refer to https://www.a-quest.com/archive/manual/aqtk2_lnx_man.pdf for more info.\n");
    return -1;
  }
  // Phont データの開放(音声合成が終わったら開放できる)
  free(pPhont);

  // 音声データ(wavフォーマット）の出力
  FILE *fp = fopen(output_fname, "wb");
  fwrite(wav, 1, size, fp);
  fclose(fp);
  // Synthe()で生成した音声データは、使用後に呼び出し側で解放する
  AquesTalk2_FreeWave (wav);
  return 0;
 }

// ファイルの読み込み
void * file_load(const char * file, int * psize)
{
  FILE *fp;
  char *data;
  struct stat st;
  *psize = 0;
  if( stat(file, &st)!=0) return NULL;
  if((data=(char *)malloc(st.st_size))==NULL){
    fprintf(stderr,"can not alloc memory(file_load)¥n");
    return NULL;
  }
  if((fp=fopen(file,"rb"))==NULL) {
    free(data);
    perror(file);
    return NULL;
  }
  if(fread(data, 1, st.st_size, fp)<(unsigned)st.st_size) {
    fprintf(stderr,"can not read data (file_load)¥n");
    free(data);
    fclose(fp);
    return NULL;
  }
  fclose(fp);
  *psize = st.st_size;
  return data;
}
