#include <stdio.h>
#include <dir.h>
#include <stdlib.h>
#include <io.h>

/* Sabit Tançmlamalarç */
#define TEMP_FILE "~$temp01.tmp"

/* Global Tançmlamalar */
struct ffblk info;
FILE *TempFile;

/* Yapç Tançmlamalarç */
struct _DATE {
	unsigned day:5;
	unsigned month:4;
	unsigned year:7; /* Yazdçrçrken 1980 ile topla */
} DATE;

struct _TIME {
	unsigned hour : 5;
	unsigned minute : 6;
	unsigned second : 5; /* Yazdçrçrken 2 ile  áarp */
} TIME;

/* Fonksiyon Prototipleri */
int Record(FILE *fp, char *Path);
int GetRec(int n, struct ffblk *pinfo);
void DispRec(int total, FILE *fp);
void SwapRec(int i, int k);

/* Programçn Baülangçcç */

void main(int argc, char *argv[])
{
	int total;
	char *p;

	if (argc == 1) {
		printf("Kullançm: List.exe <Yol veya Joker Karakter>\n"
			"ôrnek:  List *.*\n");
		exit(1);
	}
	if (argc > 2) {
		printf("Äok fazla parametre\n");
		exit(2);
	}

	if ((TempFile = fopen(TEMP_FILE, "w+b")) == NULL) {
		printf("Sistem hatasç\n");
		exit(1);
	}
	clrscr();
	total = Record(TempFile, argv[1]);
	DispRec(total, TempFile);
	fclose(TempFile);
	unlink(TEMP_FILE);
}

int Record(FILE *fp, char *Path)
{
	int count, result;

	result = findfirst(Path, &info, 0);
	if ((result) || access(TEMP_FILE, 0)) {
		printf("Dosya bulunamadç\n");
		fclose(fp);
		unlink(TEMP_FILE);
		exit(1);
	}

	fseek(fp, 0, SEEK_SET);
	fwrite(&info, sizeof(struct ffblk), 1, fp);
	for (count = 2; !findnext(&info); count++) {
		if (stricmp(info.ff_name, TEMP_FILE) == 0) {
			count--; continue;
		}
		fseek(fp, (count - 1) * sizeof(struct ffblk), SEEK_SET);
		fwrite(&info, sizeof(struct ffblk), 1, fp);
	}
	return count - 1;
}
int GetRec(int n, struct ffblk *pinfo)
{
	fseek (TempFile, n * sizeof(struct ffblk), SEEK_SET);
	fread (pinfo, sizeof(struct ffblk), 1, TempFile);
}
void SwapRec(int i, int k)
{
	struct ffblk x, y;

	/* Diskten iki kayçt okunuyor */
	fseek(TempFile, i * sizeof(struct ffblk), SEEK_SET);
	fread(&x, sizeof(struct ffblk), 1, TempFile);
	fseek(TempFile, k * sizeof(struct ffblk), SEEK_SET);
	fread(&y, sizeof(struct ffblk), 1, TempFile);

	/* Kayçtlar Yer Deßiütiriyor */
	fseek(TempFile, i * sizeof(struct ffblk), SEEK_SET);
	fwrite(&y, sizeof(struct ffblk), 1, TempFile);
	fseek(TempFile, k * sizeof(struct ffblk), SEEK_SET);
	fwrite(&x, sizeof(struct ffblk), 1, TempFile);
}

void DispRec(int total, FILE *fp)
{
	int i, x, y;
	unsigned long size = 0;
	struct ffblk pinfo, temp;
	struct _DATE *date;

	for (x = 0; x < total - 1; ++x) {
		for (y = 0; y < total - x - 1; ++y) {
			GetRec(y, &pinfo);
			GetRec((y + 1), &temp);
			if (stricmp(pinfo.ff_name, temp.ff_name) > 0)
				SwapRec(y + 1, y);
		}
	}

	for (i = 0; i < total; i++) {
		fseek(fp, i * sizeof(struct ffblk), SEEK_SET);
		fread(&pinfo, sizeof(struct ffblk), 1, fp);
		date = (struct _DATE*) &pinfo.ff_fdate;
		printf("%-13s %10ld ", pinfo.ff_name, pinfo.ff_fsize);
		printf("%02d/%02d/%02d\n", date->day, date->month, date->year + 1980);
		size += pinfo.ff_fsize;
	}
	printf("%d dosya  %lu Byte listelendi..\n", total, size);
}
