#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdint.h>

#ifndef MAX_PATH
#define MAX_PATH 4096
#endif

#if 0
#define DEBUG 1
#endif

typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;

#define IMG_SIZE 1474560

BYTE img[IMG_SIZE];
BYTE secBuf[512];

static void throwError(char *message)
{
	printf("Error: %s\n", message);
	exit(0);
}

static void readImage(char *imgName)
{
	FILE *fp;
	if ((fp = fopen(imgName, "rb")) == NULL)
		throwError("Can't read image file");
	fread(img, 1, IMG_SIZE, fp);
	fclose(fp);
}

static void writeImage(char *imgName)
{
	FILE *fp;
	if ((fp = fopen(imgName, "wb")) == NULL)
		throwError("Can't write image file");
	fwrite(img, 1, IMG_SIZE, fp);
	fclose(fp);
}

static void putBYTE(int addr, BYTE val)
{
	if (addr < 0 || addr >= IMG_SIZE)
		return;
	img[addr] = val;
}

static void putWORD(int addr, WORD val)
{
	if (addr < 0 || (addr + 1) >= IMG_SIZE)
		return;
	img[addr + 0] = (BYTE)(val & 0x00FF);
	img[addr + 1] = (BYTE)(val >> 8);
}

static void putDWORD(int addr, DWORD val)
{
	if (addr < 0 || (addr + 3) >= IMG_SIZE)
		return;
	img[addr + 0] = (BYTE)  (val & 0x000000FF);
	img[addr + 1] = (BYTE) ((val & 0x0000FF00) >> 8);
	img[addr + 2] = (BYTE) ((val & 0x00FF0000) >> 16);
	img[addr + 3] = (BYTE)  (val >> 24);
}

static BYTE getBYTE(int addr)
{
	if (addr < 0 || addr >= IMG_SIZE)
		return 0;
	return img[addr];
}

static WORD getWORD(int addr)
{
	if (addr < 0 || (addr + 1 ) >= IMG_SIZE)
		return 0;
	return ((WORD) img[addr] | (((WORD) img[addr + 1]) << 8));
}

static DWORD getDWORD(int addr)
{
	if (addr < 0 || (addr + 1) >= IMG_SIZE)
		return 0;
	return (  (DWORD) img[addr + 0] |
		(((DWORD) img[addr + 1])<< 8) |
		(((DWORD) img[addr + 2])<< 16) |
		(((DWORD) img[addr + 3])<< 24));
}

static void zeroImage(void)
{
	memset(img, 0, IMG_SIZE);
}

static void readBootSec(char *bsecName)
{
	FILE *fp;
	if ((fp = fopen(bsecName, "rb")) == NULL)
		throwError("Can't read boot sector file");
	fread(img, 1, 512, fp);
	fclose(fp);
}

static void putString(int addr, char *str)
{
	int i;
	for (i = 0; i < strlen(str); i++)
		putBYTE(addr + i, str[i]);
}

static void putFAT_Element(int fatAddr, int num, int val)
{
	int addr;
	addr = fatAddr + num * 3 / 2;
	if (num & 1) {
		putWORD(addr, (getWORD(addr) & 0x000F) | (val << 4));
	}
	else {
		putWORD(addr, (getWORD(addr) & 0xF000) | val);
	}
}

static int getFAT_Element(int fatAddr, int num)
{
	int addr;
	addr = fatAddr + num * 3 / 2;
	if (num & 1) {
		return (getWORD(addr) >> 4);
	}
	return (getWORD(addr) & 0x0FFF);
}

static void initImage(char *bsecName, char *imgName)
{
	int bps, eoc;
	int fatStart, fat2Start, rootStart, secPerRoot, dataStart;
	int fatAddr;

	printf("Initializing image ...\n");

	zeroImage();
	readBootSec(bsecName);

	putString(0x03, "OrzKernl");	/* BS_OEMName */
	putWORD(0x0B, 512);		/* BPB_BytesPerSec */
	putBYTE(0x0D, 1);		/* BPB_SecPerClust */
	putWORD(0x0E, 1);		/* BPB_RsvdSecCnt */
	putBYTE(0x10, 2);		/* BPB_NumFATs */
	putWORD(0x11, 0xE0);		/* BPB_RootEntCnt */
	putWORD(0x13, 0x0B40);		/* BPB_TotSec16 */
	putBYTE(0x15, 0xF0);		/* BPB_Media */
	putWORD(0x16, 9);		/* BPB_FATSz16 */
	putWORD(0x18, 18);		/* BPB_SecPerTrk */
	putWORD(0x1A, 2);		/* BPB_NumHeads */
	putDWORD(0x1C, 0);		/* BPB_HiddSec */
	putDWORD(0x20, 0);		/* BPB_TotSec32 */
	putBYTE(0x24, 0);		/* BS_DrvNum */
	putBYTE(0x25, 0);		/* BS_Reserved1 */
	putBYTE(0x26, 0x29);		/* BS_BootSig */
	putDWORD(0x27, 0x00162244);	/* BS_VolID */
	putString(0x2B, "Orz Disk   ");	/* BS_VolLab */
	putString(0x36, "FAT12   ");	/* BS_FilSysType */
	putWORD(0x01FE, 0xAA55);	/* End */

	bps = getWORD(0x0B);	/* bytesPerSec */
	eoc = 0xFFF;		/* end of cluster-chain */

	/* fatStart = hiddenSectors + resSectors */
	fatStart = getDWORD(0x1C) + getWORD(0x0E);
	/* fat2Start = fatStart + secPerFAT */
	fat2Start = fatStart + (int) getWORD(0x16);
	/* rootStart = fatCopies * sectPerFAT + fatStart */
	rootStart = (int) getBYTE(0x10) * (int) getWORD(0x16) + fatStart;
	/* secPerRoot = rootEntCnt * 32 / bytesPerSec */
	secPerRoot = (int) getWORD(0x11) * 32 / bps;
	/* dataStart = rootStart + secPerRoot */
	dataStart = rootStart + secPerRoot;

#ifdef DEBUG
	printf("DBG: rootStart=0x%04X, secPerRoot=0x%04X, dataStart=0x%04X\n",
		rootStart, secPerRoot, dataStart);
#endif

	fatAddr = fatStart * bps;

	putFAT_Element(fatAddr, 0, 0xFF8);
	putFAT_Element(fatAddr, 1, eoc);

	writeImage(imgName);
}

static void imageInfo(char *imgName)
{
	int i;
	int bps, eoc;
	int fatStart, fat2Start, rootStart, secPerRoot, dataStart;
	int fatAddr;

	printf("Image Info...\n");

	readImage(imgName);

	bps = getWORD(0x0B);	/* bytesPerSec */
	eoc = 0xFFF;		/* end of clusterchain */

	/* fatStart = hiddenSectors + resSectors */
	fatStart = getDWORD(0x1C) + getWORD(0x0E);
	/* fat2Start = fatStart + secPerFAT */
	fat2Start = fatStart + (int) getWORD(0x16);
	/* rootStart = fatCopies * sectPerFAT + fatStart */
	rootStart = (int) getBYTE(0x10) * (int) getWORD(0x16) + fatStart;
	/* secPerRoot = rootEntCnt * 32 / bytesPerSec */
	secPerRoot = (int) getWORD(0x11) * 32 / bps;
	/* dataStart = rootStart + secPerRoot */
	dataStart = rootStart + secPerRoot;

	fatAddr = fatStart * bps;

	for (i = 0; i < 64; i++) {
		if (i > 0)
			printf(" ");
		printf("%03X", getFAT_Element(fatAddr, i));
	}

	printf("\n");
}

static long getFileSize(char *fName)
{
	FILE *fp;
	long fsize;
	if ((fp = fopen(fName, "rb")) == NULL)
		throwError("Error opening file");
	fseek(fp, 0, SEEK_END);
	fsize = ftell(fp);
	fclose(fp);
	return fsize;
}

static int getFreeFAT_Element(int fatAddr, int clustCnt)
{
	int i;
	for (i = 2; i < clustCnt; i++) {
		if (getFAT_Element(fatAddr, i) == 0)
			return i;
	}
	return -1;
}

static WORD getTime(struct tm *tme)
{
	return ((tme->tm_hour << 11) |
		(tme->tm_min << 5) |
		(tme->tm_sec / 2));
}

static WORD getDate(struct tm *tme)
{
	return (((tme->tm_year - 80) << 9) |
		 (tme->tm_mon << 5) |
		(tme->tm_mday));
}

static int findFile(int rootAddr, int rootEnt, BYTE *fName)
{
	int i, j, addr;

	for (i = 0, addr = rootAddr; i < rootEnt; i++, addr += 32) {
		for (j = 0; j < 11; j++) {
			if (getBYTE(addr + j) != fName[j])
				break;
		}
		if (j >= 11)
			return i;
	}

	return -1;
}

static void writeBufToCluster(int dataAddr, int clustSize, int clust)
{
	int i, addr;
	addr = dataAddr + (clust - 2) * clustSize;
	for (i = 0; i < sizeof(secBuf); i++)
		putBYTE(addr + i, secBuf[i]);
}

static void copyFAT(int fat1, int fat2, int cnt)
{
	int i;
	for (i = 0; i < cnt; i++)
		putFAT_Element(fat2, i, getFAT_Element(fat1, i));
}

static void addFile(char *fName, char *fiName, char *imgName)
{
	int i, j, ex, addr, fsize, clust, sz, nclust;
	int bps, eoc, rootEnt, clustCnt;
	int fatStart, fat2Start, rootStart, secPerRoot, dataStart;
	int fatAddr, rootAddr, sectPerClust;
	BYTE resName[11];
	struct tm *tme;
	time_t tmt;
	FILE *fp;

	printf("Adding file ...");
	fsize = getFileSize(fName);

	readImage(imgName);
	bps = getWORD(0x0B);
	eoc = 0xFFF;
	sectPerClust = getBYTE(0x0D);

	if (sectPerClust != 1)
		throwError("Internal error: addFile works correct only when sectPerCluster==1");

	/* clustCnt = totalSectors / sectPerCluster */
	clustCnt = (int) getWORD(0x13) / sectPerClust;
	rootEnt = getWORD(0x11);
	fatStart = getDWORD(0x1C) + getWORD(0x0E);
	fat2Start = fatStart + (int) getWORD(0x16);
	rootStart = (int) getBYTE(0x10) * (int) getWORD(0x16) + fatStart;
	secPerRoot = rootEnt * 32 / bps;
	dataStart = rootStart + secPerRoot;
	fatAddr = fatStart * bps;
	rootAddr = rootStart * bps;

#ifdef DEBUG
	printf("DBG: rootAddr=0x%04X\n", rootAddr);
#endif

	for (i = 0, j = 0, ex = 0; i < strlen(fiName) && j < 12; i++, j++) {
		if (fiName[i] == '.') {
			if (ex)
				throwError("Multiple extensions not allowed");
			for ( ; j < 8; j++)
				resName[j] = ' ';
			j--;
			ex = 1;
		}
		else {
			if (j >= 8 && ! ex) {
				for ( ; i < strlen(fiName) && fiName[i] != '.'; i++);
				continue;
			}
			resName[j] = toupper(fiName[i]);
		}
	}

	for (; j < 12; j++)
		resName[j] = ' ';

	if (findFile(rootAddr, rootEnt, resName) >= 0)
		throwError("File already exists");

	for (i = 0, addr = rootAddr; i < rootEnt; i++, addr += 32) {
		if (getBYTE(addr) == 0x00 || getBYTE(addr) == 0xE5)
			break;
	}
	if (i >= rootEnt)
		throwError("No free root entries");

	clust = getFreeFAT_Element(fatAddr, clustCnt);
	if (clust < 0)
		throwError("No free FAT entry");

	tmt = time(NULL);
	tme = localtime(&tmt);

	memcpy(&img[addr], resName, 11);	// DIR_Name			[ +0 ]
	addr += 11;
	putBYTE(addr++, 0);			// DIR_Attr			[ +11 ]
	putBYTE(addr++, 0);			// DIR_NTRes			[ +12 ]
	putBYTE(addr++, 0);			// DIR_CrtTimeTenth (FAT32)	[ +13 ]
	putWORD(addr, 0);			// DIR_CrtTime (FAT32)		[ +14 ]
	addr += 2;
	putWORD(addr, 0);			// DIR_CrtDate (FAT32)		[ +16 ]
	addr += 2;
	putWORD(addr, 0);			// DIR_LstAccDate (FAT32)	[ +18 ]
	addr += 2;
	putWORD(addr, 0);			// DIR_FstClasHl (FAT32)	[ +20 ]
	addr += 2;
	putWORD(addr, getTime(tme));		// DIR_WrtTime			[ +22 ]
	addr += 2;
	putWORD(addr, getDate(tme));		// DIR_WrtDate			[ +24 ]
	addr += 2;
	putWORD(addr, clust);			// DIR_FstClusLo		[ +26 ]
	addr += 2;
	putDWORD(addr, fsize);			// DIR_FileSize			[ +28 ]

	if ((fp = fopen(fName, "rb")) == NULL)
		throwError("Error reading file");
	sz = fsize;

	do {
		sz -= fread(secBuf, 1, sectPerClust * bps, fp);
		writeBufToCluster(dataStart*bps, sectPerClust * bps, clust);
		if (sz) {
			putFAT_Element(fatAddr, clust, eoc); /* NOTE:
				If not set here, getFreeFAT_Element would get
				this element instead. */
			nclust = getFreeFAT_Element(fatAddr, clustCnt);
			if (nclust < 0)
				throwError("No free FAT entries to store file");
			putFAT_Element(fatAddr, clust, nclust);
			clust = nclust;
		}
		else {
			putFAT_Element(fatAddr, clust, eoc);
		}
	} while (sz);

	fclose(fp);

	copyFAT(fatAddr, fat2Start * bps, clustCnt);
	writeImage(imgName);
}

static void usage()
{
	printf(
"Usage: \n"
"Init image:\n"
"\t mkorzimg b bootsec.bin orzmicro.img\n"
"Add a file into image:\n"
"\t mkorzimg a filename IMAGE.EXT orzmicro.img\n"
"Create image information:\n"
"\t mkorzimg i orzmicro.img\n"
	);
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		usage();
		return 1;
	}

	if (! strcmp(argv[1], "b")) 
		initImage(argv[2], argv[3]);
	else if (! strcmp(argv[1], "a"))
		addFile(argv[2], argv[3], argv[4]);
	else if (! strcmp(argv[1], "i")) 
		imageInfo(argv[2]);
	else
		throwError("Incorrect parameters");

	printf("... ok\n");
	return 0;
}
