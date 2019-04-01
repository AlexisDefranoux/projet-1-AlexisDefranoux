void printArray(struct tablo * tmp, int size) : non parallel
struct tablo * allocateTablo(int size) : non parallel
void freeTab(struct tablo* tmp) : non parallel
void copyTabAndReverseTab(struct tablo* source,struct tablo* doubleSizeTab, struct tablo* doubleSizeTabReversed) : parallel
void copyTabSsumAndPsum(struct tablo* ssum, struct tablo* psum, struct tablo* doubleSizeTab, struct tablo* doubleSizeTabReversed) : parallel
void montee(struct tablo* psum, struct tablo* a) : parallel
void descente(struct tablo * a, struct tablo * b) : parallel
void finalPrefix(struct tablo* a, struct tablo* b, struct tablo* psum) : parallel
void finalSuffix(struct tablo* a, struct tablo* b, struct tablo* ssum) : non parallel
void parseFile(char *filePath, struct tablo * myList) : non parallel
void sumPrefix(struct tablo* psum, struct tablo* source) : parallel
void sumSuffix(struct tablo* ssum, struct tablo* source) : parallel
void monteeMax(struct tablo* psum, struct tablo* a) : parallel
void descenteMax(struct tablo * a, struct tablo * b) : parallel
void finalPmax(struct tablo* a, struct tablo* b, struct tablo* pmax) : parallel
void finalSmax(struct tablo* a, struct tablo* b, struct tablo* smax) : non parallel
void pMax(struct tablo* smax, struct tablo* psum) : non parallel
void sMax(struct tablo* pmax, struct tablo* ssum) : non parallel
void mFinal(struct tablo* m, struct tablo* ms, struct tablo* mp, struct tablo* source) : parallel
void findAndPrint(struct tablo* m, struct tablo* source) : non parallel
