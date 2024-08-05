#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// definim structura ce contine matricea cu cele 3 campuri
typedef struct RGBMatrix {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} RGBMatrix;

// definim structura ce contine arborele
typedef struct Quadtree {
    int level;
    unsigned char frunza;
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    struct Quadtree *p1;
    struct Quadtree *p2;
    struct Quadtree *p3;
    struct Quadtree *p4;
} Quadtree;

// definimi structurile pentru coada ce ne va ajuta
// sa parcurgem arborele in latime
typedef struct NodeCoada {
    Quadtree *node;
    struct NodeCoada *next;
} NodeCoada;

typedef struct coada {
    struct NodeCoada *first;
    struct NodeCoada *last;
} coada;

// functie care initializeaza coada
void initCoada(coada *queue) {
    queue->first = NULL;
    queue->last = NULL;
}

// functie care verifica daca coada este vida
int EmptyQueue(coada *queue) {
    if (queue->first == NULL) {
        return 0;
    } else {
        return 1;
    }
}

// functie care adauga un element in coada
void AddQueue(Quadtree *tree, coada *queue) {
    NodeCoada *new = malloc(sizeof(NodeCoada));
    new->node = tree;
    new->next = NULL;
    if(queue->first == NULL) {
        queue->first = new;
        queue->last = new;
    } else {
        queue->last->next = new;
        queue->last = new; 
    }
}

// functie care extrage si sterge un element din coada
Quadtree* RemoveQueue(coada *queue) {
    NodeCoada *c = queue->first;
    Quadtree *tree;
    if (c == NULL) {
        return NULL;
    }
    tree = queue->first->node;
    if(queue->first == queue->last) {
        queue->first = NULL;
        queue->last = NULL;
    } else {
        queue->first = queue->first->next;
    }
    free(c);
    return tree;
}

// functie care parcurge arborele pe verticala
void ParcurgereVerticala(Quadtree *tree, FILE *f) {
    Quadtree *t;
    coada *queue = malloc(sizeof(coada));
    initCoada(queue);
    AddQueue(tree, queue);
    // cat timp exista elemente in coada, inseamna ca
    // exista elemente nevizitate
    while (EmptyQueue(queue) == 1) {
        // extragem elementul
        t = RemoveQueue(queue);
        // afisam datele
        if (t->frunza == 0) {
            fwrite(&t->frunza, sizeof(unsigned char), 1, f);
        } else {
            fwrite(&t->frunza, sizeof(unsigned char), 1, f);
            fwrite(&t->red, sizeof(unsigned char), 1, f);
            fwrite(&t->green, sizeof(unsigned char), 1, f);
            fwrite(&t->blue, sizeof(unsigned char), 1, f);
        }
        // adaugam in coada copiii nodului respectiv
        if (t->p1 != NULL) {
            AddQueue(t->p1, queue);
        }
        if (t->p2 != NULL) {
            AddQueue(t->p2, queue);
        }
        if (t->p3 != NULL) {
            AddQueue(t->p3, queue);
        }
        if (t->p4 != NULL) {
            AddQueue(t->p4, queue);
        }
    }
    // eliberam memoria alocata cozii
    free(queue);
}

// functie care initializeaza arborele
void initTree (Quadtree **tree)
{
    (*tree) = malloc(sizeof(Quadtree));
    (*tree)->p1 = NULL;
    (*tree)->p2 = NULL;
    (*tree)->p3 = NULL;
    (*tree)->p4 = NULL;
    (*tree)->red = 0;
    (*tree)->green = 0;
    (*tree)->blue = 0;
}

int isQuadTreeFull(Quadtree* tree) {
	if (tree->p1 == NULL || tree->p2 == NULL ||
        tree->p3 == NULL || tree->p4 == NULL) {
        return 0;
    }
	return 1;
}

// eliberam memoria arborelui
void freeMem(Quadtree **tree) {
    if ((*tree)->p1 != NULL) {
        freeMem(&(*tree)->p1);
        freeMem(&(*tree)->p2);
        freeMem(&(*tree)->p3);
        freeMem(&(*tree)->p4);
    }
    free(*tree);
}

// functie care creeaza arborele
// primeste ca argument primul nod al arborelui, matricea, dimensiunea acesteia,
// prima pozitie a matricii, factorul si nr care este numarul
// matrici care au scorul mai mic ca factorul, level ce retine numarul
// de niveluri ale arborelui si Max latura maxima a unei matrici
// ce ramane nedivizata
void createQuadtree (Quadtree *tree, RGBMatrix **a, int size, int poz1,
int poz2, int factor, int *nr, int *level, int *Max)
{
    // calculam culorile medii si scorul similaritatii
    unsigned long long red = 0, green = 0, blue = 0;
    unsigned long long mean = 0;
    int i, j;
    for (i = poz1; i < poz1 + size; i++) {
        for (j = poz2; j < poz2 + size; j++) {
            red = red + a[i][j].red;
            green = green + a[i][j].green;
            blue = blue + a[i][j].blue;
        }
    }
    unsigned long long power;
    power = size * size;
    red = red / power;
    green = green / power;
    blue = blue / power;
    for (i = poz1; i < poz1 + size; i++) {
        for (j = poz2; j < poz2 + size; j++) {
            mean = mean + (red - a[i][j].red) * (red - a[i][j].red) +
            (green - a[i][j].green) *(green - a[i][j].green) +
            (blue - a[i][j].blue) * (blue - a[i][j].blue);
        }
    }
    mean = mean / (3 * power);

    // daca mean > factor, atunci cream 4 copii ai nodului tree si apelam
    // functia in fiecare dintre acestia,
    // specificand dimensiunile noilor matrici si primele pozitii ale acestora
    if (mean > (unsigned long long)factor) {
        Quadtree *new1, *new2, *new3, *new4;
        initTree(&new1);
        initTree(&new2);
        initTree(&new3);
        initTree(&new4);
        tree->frunza = 0;
        tree->p1 = new1;
        tree->p2 = new2;
        tree->p3 = new3;
        tree->p4 = new4;
        new1->level = tree->level + 1;
        new2->level = tree->level + 1;
        new3->level = tree->level + 1;
        new4->level = tree->level + 1;
        
        // deoarece salvam in campul level al fiecarui nod al arborelui
        // nivelul pe care se afla, putem calcula nivelul maxim,
        // adica inaltimea arborelui ce va fi salvata in level
        if (tree->level + 1 > *level) {
            *level = tree->level + 1;
        }

        createQuadtree(new1, a, size/2, poz1, poz2, factor,
        &(*nr), &(*level), &(*Max));
        createQuadtree(new2, a, size/2, poz1, poz2 + size/2, factor,
        &(*nr), &(*level), &(*Max));
        createQuadtree(new3, a, size/2, poz1 + size/2, poz2 + size/2, factor,
        &(*nr), &(*level), &(*Max));
        createQuadtree(new4, a, size/2, poz1 + size/2, poz2, factor,
        &(*nr), &(*level), &(*Max));
    } else {
        // aflam cea mai mare latura a unui patrat care ramane nedivizat si salvam in Max
        if (size > (*Max)) {
            (*Max) = size;
        }

        // numaram cate patrate raman nedivizate si salvam in nr rezultatul
        (*nr)++;
        tree->frunza = 1;
        tree->red = (unsigned char)red;
        tree->green = (unsigned char)green;
        tree->blue = (unsigned char)blue;
    }
}

// functie care citeste arborele dintr-un fisier
Quadtree* decompressedQuadtree (FILE* f) {
    coada* queue = calloc(1, sizeof(coada));
    Quadtree* tree = calloc(1, sizeof(Quadtree));
    Quadtree* root = tree;

    fread(&tree->frunza, sizeof(char), 1, f);

    if (tree->frunza == 1) {
        fread(&tree->red, sizeof(char), 1, f);
		fread(&tree->green, sizeof(char), 1, f);
		fread(&tree->blue, sizeof(char), 1, f);
    }

    while (1) {
        Quadtree* child = calloc(1, sizeof(Quadtree));

        if (fread(&child->frunza, sizeof(char), 1, f) == 0) {
			freeMem(&child);
			break;
		}

        if (child->frunza == 0) {
			AddQueue(child, queue);
        }

		if (child->frunza == 1) {
			fread(&child->red, sizeof(char), 1, f);
			fread(&child->green, sizeof(char), 1, f);
			fread(&child->blue, sizeof(char), 1, f);
		}

		if (isQuadTreeFull(tree)) {
			tree = queue->first->node;
			RemoveQueue(queue);
		}

		if (tree->p1 == NULL)
			tree->p1 = child;
		else if (tree->p2 == NULL)
			tree->p2 = child;
		else if (tree->p3 == NULL)
			tree->p3 = child;
		else if (tree->p4 == NULL)
			tree->p4 = child;
    }

    free(queue);

    return root;
}

// functie care construieste arborele intr-o matrice
void buildQuadtreeMatrix(Quadtree* tree, RGBMatrix** a, int size, int x, int y) {
	if (tree->frunza == 0) {
		int childSize = size / 2;
		buildQuadtreeMatrix(tree->p1, a, childSize, x, y);
		buildQuadtreeMatrix(tree->p2, a, childSize, x, y + childSize);
		buildQuadtreeMatrix(tree->p3, a, childSize, x + childSize, y + childSize);
		buildQuadtreeMatrix(tree->p4, a, childSize, x + childSize, y);
	}

	if (tree->frunza == 1) {
		for (int i = x; i < x + size; i++) {
			for (int j = y; j < y + size; j++) {
				a[i][j].red = tree->red;
				a[i][j].green = tree->green;
				a[i][j].blue = tree->blue;
			}
		}
	}
}

int main(int argc, char *argv[]) {

    // pentru cerintele 1 si 2
    if(argc == 5) {
        FILE *f = fopen(argv[3], "rb");
        FILE *fout = fopen(argv[4], "wb");

        int width, height, MaxNumber;
        fscanf(f, "P6\n");
        fscanf(f, "%d %d\n", &width, &height);
        fscanf(f, "%d", &MaxNumber);
        char x[2];
        fread(x, sizeof(char), 1, f);
        RGBMatrix **a = (RGBMatrix**)malloc(height * sizeof(RGBMatrix*));
        int i, j;
        // alocam dinamic memorie pentru matrice
        for (i = 0; i < height; i++) {
            a[i] = (RGBMatrix*)malloc(width * sizeof(RGBMatrix));
        }
        for(i = 0; i < width; i++) {
            for (j = 0; j < width; j++) {
                fread(&a[i][j].red, sizeof(unsigned char), 1, f);
                fread(&a[i][j].green, sizeof(unsigned char), 1, f);
                fread(&a[i][j].blue, sizeof(unsigned char), 1, f);
            }
        }

        Quadtree *tree;
        initTree(&tree);
        tree->level = 1;
    
        int factor = 0, nr = 0, level = 1, Max=0;
        // construim factorul
        factor = atoi(argv[2]);
        createQuadtree(tree, a, width, 0, 0, factor, &nr, &level, &Max);

        // cerinta 1
        if(strcmp(argv[1], "-c1") == 0) {
            fprintf(fout, "%d\n%d\n%d\n", level, nr, Max);
        }

        // cerinta 2
        if(strcmp(argv[1], "-c2") == 0) {
            fwrite(&width, sizeof(unsigned int), 1, fout);
            ParcurgereVerticala(tree, fout);
        }

        // eliberam memoria arborelui si a matricii
        freeMem(&tree);
        for (i = 0; i < width; i++) {
            free(a[i]);
        }
        free(a);

        fclose(f);
        fclose(fout);
    }

    // cerinta 3
    if (strstr(argv[1], "-d")) {
        FILE *f = fopen(argv[argc - 2], "rb");
        FILE *fout = fopen(argv[argc - 1], "wb");

        int size;

        fread(&size, sizeof(int), 1, f);

		fprintf(fout, "P6\n");
		fprintf(fout, "%d %d\n", size, size);
		fprintf(fout, "255\n");

        Quadtree* tree = decompressedQuadtree(f);

        RGBMatrix** a = calloc(size, sizeof(RGBMatrix*));
        int i;
        for (i = 0; i < size; i++) {
            a[i] = calloc(size, sizeof(RGBMatrix));
        }

        buildQuadtreeMatrix(tree, a, size, 0, 0);

        for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				fwrite(&a[i][j].red, sizeof(char), 1, fout);
				fwrite(&a[i][j].green, sizeof(char), 1, fout);
				fwrite(&a[i][j].blue, sizeof(char), 1, fout);
			}
		}

        // eliberam memoria arborelui si a matricii
        freeMem(&tree);
        for (i = 0; i < size; i++) {
            free(a[i]);
        }
        free(a);

        fclose(f);
        fclose(fout);
    }

    return 0;
}
