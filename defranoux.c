#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <math.h>

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

void montee(struct tablo* psum, struct tablo* destination, struct tablo* source) {
  #pragma omp parallel for
  for (int i = 0; i < normalSize; ++i)
    destination->tab[normalSize + i] = source->tab[i];

  m = log2(normalSize);
  for (int l = m-1; l >= 0; --l) {

    int init = pow(2, l);
    int end = (pow(2, l+1) -1);

    #pragma omp parallel for
    for (int j = init; j <= end; ++j)
      destination->tab[j] = destination->tab[2*j] + destination->tab[2*j+1];
  }
}

void descente(struct tablo * a, struct tablo * b) {
  b->tab[0] = 0;
  b->tab[1] = 0;

  for (int l = 1; l <= m; ++l) {
    int init = pow(2, l);
    int end = (pow(2, l+1) -1);

    #pragma omp parallel for
    for (int j = init; j <= end; ++j) {
      if (j%2 == 0) b->tab[j] = b->tab[j/2];
      else b->tab[j] = b->tab[j/2] + a->tab[j-1];
    }
  }
}

void final(struct tablo* a, struct tablo* b, struct tablo* psum) {
  int init = pow(2, m);
  int end = (pow(2, m+1) -1);

  #pragma omp parallel for
  for (int j = init; j <= end; ++j)
      psum->tab[j-normalSize] = b->tab[j] + a->tab[j];
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

void sumPrefix(struct tablo* psum, struct tablo* source){
  struct tablo* a = allocateTablo(doubleSize);
  montee(psum, a, source);

  struct tablo* b = allocateTablo(doubleSize);
  descente(a, b);
  final(a,b, psum);
  free(a->tab);
  free(a);
  free(b->tab);
  free(b);
}

void sumSuffix(struct tablo* ssum, struct tablo* source) {
  ssum->tab[normalSize-1] = source->tab[normalSize-1];
  for (int i = normalSize-2; i >= 0; --i)
    ssum->tab[i] = source->tab[i] + ssum->tab[i+1];
}

void sMax(struct tablo* smax, struct tablo* psum) {
  smax->tab[normalSize-1] = psum->tab[normalSize-1];
  for (int i = normalSize-2; i >= 0; --i){
    if(smax->tab[i+1] < psum->tab[i])
      smax->tab[i] = psum->tab[i];
    else
      smax->tab[i] = smax->tab[i+1];
  }
}

void pMax(struct tablo* pmax, struct tablo* ssum) {
  pmax->tab[0] = ssum->tab[0];
  for (int i = 1; i <= normalSize-1; ++i){
    if(pmax->tab[i-1] < ssum->tab[i]) 
      pmax->tab[i] = ssum->tab[i];
    else 
      pmax->tab[i] = pmax->tab[i-1];
  }
}

void mFinal(struct tablo* m, struct tablo* pmax, struct tablo* ssum, struct tablo* smax, struct tablo* psum, struct tablo* source) {
  #pragma omp parallel for
  for (int i = 0; i <= normalSize-1; ++i)
    m->tab[i] = 
      (pmax->tab[i] - ssum->tab[i] + source->tab[i]) +
      (smax->tab[i] - psum->tab[i] + source->tab[i]) -
      source->tab[i];

  free(pmax->tab);
  free(pmax);
  free(ssum->tab);
  free(ssum);
  free(smax->tab);
  free(smax);
  free(psum->tab);
  free(psum);
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
  for (int i = start; i <= end; ++i){
    printf("%d ", source->tab[i]);
  }

  free(m->tab);
  free(m);
}

int main(int argc, char **argv) {
  struct tablo source;
  parseFile(argv[1], &source);

  struct tablo* psum = allocateTablo(normalSize);
  sumPrefix(psum, &source);

  struct tablo* ssum = allocateTablo(normalSize);
  sumSuffix(ssum, &source);

  struct tablo* smax = allocateTablo(normalSize);
  struct tablo* pmax = allocateTablo(normalSize);
  #pragma omp parallel sections
  {
    #pragma omp section
    {
      sMax(smax, psum);
    }
    #pragma omp section
    {
      pMax(pmax, ssum);
    }
  }

  struct tablo* m = allocateTablo(normalSize);
  mFinal(m, pmax, ssum, smax, psum, &source);

  findAndPrint(m, &source);

  return 0;
}