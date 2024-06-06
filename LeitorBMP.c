#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>



typedef struct{

     int W; //largura da imagem em pixel.
     int H; //altura da imagem em pixel.
     short int b_p_px; //bits_por_pixel.

} Imagem;


FILE *bmpopen(char[64]);

unsigned int bmp_header(FILE *);

Imagem DIB_header(FILE *);

void bmp_read(FILE *, Imagem, unsigned char *);

unsigned char pixel_val(unsigned char *);


/*
    O programa abre uma imagem bitmap e imprime ela no terminal usando apenas pontos e espaços.
    A saida possui as mesmas dimensoes da imagem original.
    Cada ponto representa um bit com valor maior que 64, o ponto, um menor ou igual a 64.
    Ainda nao sei qual eh o tamanho maximo da imagem.

    O programa ainda nao suporta compressao nem imagens que nao comecem com os caracteres "BM".
*/

FILE *bmpopen (char str[64])
{

    FILE *p = fopen(str,"r");

    if (p == NULL)
    {
        for (int i=0; i<strlen(str); i++)
        {
            if (str[i] == '_')
            {
                str[i] = ' ';
            }
        }
        p = fopen(str, "r");
    }

    return p;

}

/*
    A funcao bmp_header() le todas as informacoes do cabecalho do bitmap e as escreve na tela como uma forma
    de garantir que tudo deu certo.
    Ela retorna a posicao do inicio da matriz que contem as informacoes da imagem, que eh a unica
    informacao que queremos do cabecalho.
*/

unsigned int bmp_header (FILE *bmp)
{

    char bm[3];
    unsigned int size;
    unsigned int array_st;

    fread(bm, 1, 2, bmp);
    bm[3] = '\0';

    if(bm[0] == 'B' && bm[1] == 'M'){
        printf("O arquivo foi aberto corretamente\n");
    }else{
        printf("O arquivo nao foi aberto corretamente ou o cabecalho nao eh suportado.\n");
        exit(0);
    }

    fseek(bmp, 2, SEEK_SET);
    fread(&size, 4, 1, bmp);
    fseek(bmp, 10, SEEK_SET);
    fread(&array_st, 4, 1, bmp);

    printf("\n\n\nBM: %s\nTamanho do arquivo: %u  bytes\nPosicao do inicio da imagem: %u\n",bm, size, array_st);

    return array_st;

}


/*
    A funcao DIB_header() le o cabecalho Dib da imagem e guarda o numero de bits por pixel,
    altura e largura da imagem e retorna uma struct Imagem com esses valores.
    Assim como bmpp_header(), imprime os valores lidos do cabecalho para indicar que tudo
    foi lido corretamente.
*/

Imagem DIB_header (FILE *bmp)
{

    Imagem a;
    
    int compressao;

    //long pos;

    fread(&a.W, 4, 1, bmp);
    //pos = ftell(bmp);
    //printf("Posicao atual: %ld\n", pos);

    fread(&a.H, 4, 1, bmp);
    //pos = ftell(bmp);
    //printf("Posicao atual(tem q da 22): %ld\n", pos);

    fseek(bmp, 2, SEEK_CUR);
    fread(&a.b_p_px, 2, 1, bmp);

    fread(&compressao, 4, 1, bmp);

    if(compressao == 0){
        printf("Sem compressao (%d)\n", compressao);
    }else{
        printf("Compressao(%d)\n",compressao);
    }

    return a;
}

void bmp_read (FILE *bmp, Imagem img, unsigned char *out)
{

    if(img.b_p_px != 24){
        printf("O programa nao cosegue ler quando eh diferente de 24 bits por pixel atualmente.\nBits por pixel: %hd\n", img.b_p_px);
        exit(0);
    }

    int padding;

    if(img.W%4 == 0){

        padding = 0;

    }else{
        
        int mult = img.W/4 + 1;
        
        padding = (4*mult)%img.W;

    }

    printf("Padding: %d\n", padding);

    unsigned char pixel[3];

    for(int i=0; i<img.H; i++){
        for(int j=0; j<img.W; j++){
            fread(pixel, 1, 3, bmp);
            out[(img.H - 1 - i)*img.W + j] = pixel_val(pixel);
        }
        fseek(bmp, padding, SEEK_CUR);
    }

    return;

}

unsigned char pixel_val (unsigned char *pixel)
{

    //Para imagens com apenas RGB, sem alfa nem nada.

    unsigned char val;

    if(pixel[0] <= 32 && pixel[1] <= 32 && pixel[2] <= 32){
        val = (unsigned char) ' ';
    }else if(pixel[0] <= 64 && pixel[1] <= 64 && pixel[2] <= 64){
        val = (unsigned char) '.';
    }else if(pixel[0] <= 96 && pixel[1] <= 96 && pixel[2] <= 96){
        val = (unsigned char) '"';
    }else if(pixel[0] <= 128 && pixel[1] <= 128 && pixel[2] <= 128){
        val = (unsigned int) '/';
    }else if(pixel[0] <= 160 && pixel[1] <= 160 && pixel[2] <= 160){
        val = (unsigned int) 'O';
    }else if(pixel[0] <= 192 && pixel[1] <= 192 && pixel[2] <= 192){
        val = (unsigned int) '0';
    }else if(pixel[0] <= 224 && pixel[1] <= 224 && pixel[2] <= 224){
        val = (unsigned int) '@';
    }else{
        val = (unsigned int) '#';
    }

    return val;
}


int main (int argc, char *argv[])
{

    char *path = (char *) malloc(64*sizeof(char));
    Imagem BMP;
    unsigned int arr_st;
    int offset;
    FILE *output;

/*    printf("Digite o nome do arquivo:\n");

    fgets(path, 64, stdin);

    path[strlen(path) - 1] = '\0';
*/
    FILE *bmp = bmpopen(argv[1]);

    if (bmp == NULL)
    {
        printf("O path nao esta correto ou o arquivo nao existe\n(ou o negocio so n abriu msm)\n");
        return 0;
    }

    arr_st = bmp_header(bmp);

    fseek(bmp, 14, SEEK_SET);
    fread(&offset, sizeof(int), 1, bmp);

    printf("Tamanho do DIB header: %u\n", offset);

    BMP = DIB_header(bmp);

    printf("Imagem: %dx%d\nBits por pixel: %hd\n", BMP.W, BMP.H, BMP.b_p_px);

    printf("Posicao atual do ponteiro no arquivo: %ld\n", ftell(bmp));

    unsigned char *im_bmp = (unsigned char *) malloc(BMP.W*BMP.H*sizeof(char));

    if (im_bmp == NULL)
    {
        printf("Sem memoria\n");
        abort();
    }

    fseek(bmp, arr_st, SEEK_SET);

    bmp_read(bmp, BMP, im_bmp);

    output = fopen("output.txt", "w");

    if (output != NULL)
    {

        fflush(output);

   for (int i=0; i<BMP.H; i++)
   {
    for (int j=0; j<BMP.W; j++)
    {
        fputc((int) im_bmp[i*BMP.W + j], output);
        fputc((int) ' ',output);
    }
    fputc((int) '\n', output);
   }

   //system("clear");

/*
   for(int i=0; i<BMP.H; i++){
    for(int j=0; j<BMP.W; j++){
        printf("%c ", im_bmp[i*BMP.W + j]);
    }
    printf("\n");
   }
*/

    }
    else
    {
        printf("Nao foi possivel abrir o arquivo de saida.\n");
    }

    fclose(output);
    fclose(bmp);
    free(path);
    free(im_bmp);

    return 0;

}