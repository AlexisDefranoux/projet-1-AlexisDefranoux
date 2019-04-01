#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <limits.h>

struct tablo {
  int * tab;
};

int normalSize = 0;
int doubleSize = 0;
int m = 0;

void printArray(struct tablo * tmp, int size) {
  for (int i = 0; i < size; ++i) 
    printf("%i ", tmp->tab[i]);
  printf("\n");
}

struct tablo * allocateTablo(int size) {
  struct tablo* tmp = malloc(size);
  tmp->tab = malloc(size*sizeof(int));
  return tmp;
}

void freeTab(struct tablo* tmp){
  free(tmp->tab);
  free(tmp);
}

void copyTabAndReverseTab(struct tablo* source,struct tablo* doubleSizeTab, struct tablo* doubleSizeTabReversed){
  #pragma omp parallel for
  for (int i = 0; i < normalSize; ++i){
    doubleSizeTab->tab[normalSize + i] = source->tab[i];
    doubleSizeTabReversed->tab[i+normalSize] = source->tab[normalSize-i-1];
  }
}

void copyTabSsumAndPsum(struct tablo* ssum, struct tablo* psum, struct tablo* doubleSizeTab, struct tablo* doubleSizeTabReversed){
  #pragma omp parallel for
  for (int i = 0; i < normalSize; ++i){
    doubleSizeTab->tab[normalSize + i] = ssum->tab[i];
    doubleSizeTabReversed->tab[i+normalSize] = psum->tab[normalSize-i-1];
  }
}

void montee(struct tablo* psum, struct tablo* a) {
  m = log2(normalSize);
  for (int l = m-1; l >= 0; --l) {
    int init = pow(2, l);
    int end = (pow(2, l+1) -1);
    #pragma omp parallel for
    for (int j = init; j <= end; ++j)
      a->tab[j] = a->tab[2*j] + a->tab[2*j+1];
  }
}

void descente(struct tablo * a, struct tablo * b) {
  b->tab[1] = 0;
  for (int l = 1; l <= m; ++l) {
    int init = pow(2, l);
    int end = (pow(2, l+1) -1);
    #pragma omp parallel for
    for (int j = init; j <= end; ++j) {
      if (j%2 == 0) 
        b->tab[j] = b->tab[j/2];
      else 
        b->tab[j] = b->tab[j/2] + a->tab[j-1];
    }
  }
}

void finalPrefix(struct tablo* a, struct tablo* b, struct tablo* psum) {
  #pragma omp parallel for
  for (int j = normalSize; j < doubleSize; ++j)
    psum->tab[j-normalSize] = b->tab[j] + a->tab[j];
}

void finalSuffix(struct tablo* a, struct tablo* b, struct tablo* ssum) {
  int j = doubleSize-1;
  #pragma omp parallel for
  for (int i = 0; i < normalSize; ++i){
    ssum->tab[i] = b->tab[j] + a->tab[j];
    --j;
  }
}

void parseFile(char *filePath, struct tablo * myList) {
  FILE *file;

  if ((file = fopen(filePath, "r")) == NULL){
    printf("Erreur sur l'ouverture du fichier");
    exit(1);
  }

  int nbr = 0;  
  fscanf (file, "%d", &nbr);
  while(!feof (file)){
    fscanf (file, "%d", &nbr);
    normalSize++;
  }
  
  myList->tab=malloc(normalSize*sizeof(int));
  doubleSize = normalSize*2;

  rewind(file);

  fscanf (file, "%d", &nbr);
  for(int i=0; !feof (file); ++i){
    myList->tab[i] = nbr; 
    fscanf (file, "%d", &nbr);
  }
  fclose(file);
}

void sumPrefix(struct tablo* doubleSizeTab, struct tablo* psum){
  montee(psum, doubleSizeTab);
  struct tablo* b = allocateTablo(doubleSize);
  descente(doubleSizeTab, b);
  finalPrefix(doubleSizeTab, b, psum);
  freeTab(b);
}

void sumSuffix(struct tablo* doubleSizeTabReversed, struct tablo* ssum) {
  montee(ssum, doubleSizeTabReversed);
  struct tablo* b = allocateTablo(doubleSize);
  descente(doubleSizeTabReversed, b);
  finalSuffix(doubleSizeTabReversed, b, ssum);
  freeTab(b);
}

void monteeMax(struct tablo* psum, struct tablo* a) {
  m = log2(normalSize);
  for (int l = m-1; l >= 0; --l) {
    int init = pow(2, l);
    int end = (pow(2, l+1) -1);
    #pragma omp parallel for
    for (int j = init; j <= end; ++j)
      a->tab[j] = fmax(a->tab[2*j], a->tab[2*j+1]);
  }
}

void descenteMax(struct tablo * a, struct tablo * b) {
  b->tab[1] = INT_MIN;
  for (int l = 1; l <= m; ++l) {
    int init = pow(2, l);
    int end = (pow(2, l+1) -1);
    #pragma omp parallel for
    for (int j = init; j <= end; ++j) {
      if (j%2 == 0) 
        b->tab[j] = b->tab[j/2];
      else 
        b->tab[j] = fmax(b->tab[j/2], a->tab[j-1]);
    }
  }
}

void finalPmax(struct tablo* a, struct tablo* b, struct tablo* pmax) {
  #pragma omp parallel for
  for (int j = normalSize; j < doubleSize; ++j)
    pmax->tab[j-normalSize] = fmax(b->tab[j], a->tab[j]);
}

void finalSmax(struct tablo* a, struct tablo* b, struct tablo* smax) {
  for (int i = normalSize, j = normalSize - 1; i < doubleSize; ++i, --j)
    smax->tab[j] = fmax(b->tab[i], a->tab[i]);
}

void pMax(struct tablo* doubleSizeTab, struct tablo* pmax) {
  monteeMax(pmax, doubleSizeTab);
  struct tablo* b = allocateTablo(doubleSize);
  descenteMax(doubleSizeTab, b);
  finalPmax(doubleSizeTab, b, pmax);
  freeTab(doubleSizeTab); freeTab(b);
}

void sMax(struct tablo* doubleSizeTabReversed, struct tablo* smax) {
  monteeMax(smax, doubleSizeTabReversed);
  struct tablo* b = allocateTablo(doubleSize);
  descenteMax(doubleSizeTabReversed, b);
  finalSmax(doubleSizeTabReversed, b, smax);
  freeTab(doubleSizeTabReversed); freeTab(b);
}

void mFinal(struct tablo* m, struct tablo* pmax, struct tablo* ssum, struct tablo* smax, struct tablo* psum, struct tablo* source) {
  #pragma omp parallel for
  for (int i = 0; i <= normalSize-1; ++i)
    m->tab[i] = 
      (pmax->tab[i] - ssum->tab[i] + source->tab[i]) +
      (smax->tab[i] - psum->tab[i] + source->tab[i]) -
      source->tab[i];

  freeTab(pmax); freeTab(ssum);freeTab(smax); freeTab(psum);
}

void findAndPrint(struct tablo* m, struct tablo* source){
  int max = m->tab[0];
  int start=0;
  int end=0;
  for (int i = 1; i <= normalSize-1; ++i){
    if(max < m->tab[i]){
      max = m->tab[i];
      start = i;
    }else if(m->tab[i-1] > m->tab[i] && max == m->tab[i-1])
      end = i-1;
  }

  if(max == m->tab[normalSize-1])
      end = normalSize-1;

  printf("%d ", max);
  for (int i = start; i <= end; ++i)
    printf("%d ", source->tab[i]);

  freeTab(m);
}

int main(int argc, char **argv) {
  struct tablo source;
  parseFile(argv[1], &source);

  struct tablo* psum = allocateTablo(normalSize);
  struct tablo* ssum = allocateTablo(normalSize);
  struct tablo* doubleSizeTab = allocateTablo(doubleSize);
  struct tablo* doubleSizeTabReversed = allocateTablo(doubleSize);
  copyTabAndReverseTab(&source, doubleSizeTab, doubleSizeTabReversed);

  #pragma omp parallel
  {
    #pragma omp parallel sections
    {
      #pragma omp section
      {
        sumPrefix(doubleSizeTab, psum);
      }
      #pragma omp section
      {
        sumSuffix(doubleSizeTabReversed, ssum);
      }
    }
  }

  struct tablo* smax = allocateTablo(normalSize);
  struct tablo* pmax = allocateTablo(normalSize);
  copyTabSsumAndPsum(ssum, psum, doubleSizeTab, doubleSizeTabReversed);

  sMax(doubleSizeTabReversed, smax);
  pMax(doubleSizeTab, pmax);

  struct tablo* m = allocateTablo(normalSize);
  mFinal(m, pmax, ssum, smax, psum, &source);
  findAndPrint(m, &source);

  return 0;
}