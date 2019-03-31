void printArray(struct tablo * tmp, int size) : non parallel
struct tablo * allocateTablo(int size) : non parallel
void montee(struct tablo* psum, struct tablo* destination, struct tablo* source) : parallel
void descente(struct tablo * a, struct tablo * b) : parallel
void final(struct tablo* a, struct tablo* b, struct tablo* psum) : parallel
void parseFile(char *filePath, struct tablo * myList) : non parallel
void sumPrefix(struct tablo* psum, struct tablo* source) : non parallel
void sumSuffix(struct tablo* ssum, struct tablo* source) : non parallel
void sMax(struct tablo* smax, struct tablo* psum) : parallel
void pMax(struct tablo* pmax, struct tablo* ssum) : parallel
void mFinal(struct tablo* m, struct tablo* ms, struct tablo* mp, struct tablo* source) : parallel
void findAndPrint(struct tablo* m, struct tablo* source) : non parallel
