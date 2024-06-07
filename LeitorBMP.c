#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


typedef short int sh;
typedef struct{

     int W; //largura da imagem em pixel.
     int H; //altura da imagem em pixel.
     unsigned int tam_bytes;
     sh b_p_px; //bits_por_pixel.
     sh compressao;

} Imagem;


FILE *bmpopen(char[64]);

unsigned int bmp_header(FILE *, Imagem *, int);

Imagem DIB_header(FILE *);

void bmp_read(FILE *, Imagem, unsigned char *, int);

unsigned char pixel_val(unsigned char *);

int *ler_flags(FILE **, int, char **);


/*
* ler_flags() le as flags em argv[] e cria um vetor com os valores 0 ou 1 para
* cada uma das flags, caso tenha algumas.
* Cada flag tem uma posição específica dentro desse vetor:
* -h: 0
* -e: 1
* -d: 2
* -s: 3
* -o: 4
* ...
*/
int *ler_flags(FILE **output, int n_arg, char *argv[])
{
    int *flags = (int *) calloc(5,sizeof(int));

    int tres = 0;

    if (flags == NULL)
    {
        abort();
    }

    for (int i=1; i < n_arg; i++)
    {
        if (argv[i][0] == '-')
        {

            if(argv[i][2] != '\0')
            {
                printf("LeitorBMP <nome do arquivo/caminho> -flags ...\n\n\n-d          Imprime dados da imagem\n-e          Tira os espaços entre os pontos quando sao impressos na tela.\n-h          Imprime esse texto.\n-o          Especifica um arquivo para escrever o ASCII\n");
                free(flags);
                flags = NULL;
                exit(0);
            }

            switch (argv[i][1])
            {
                case 'h':
                    //-h imprime uma tela com todas as flags possiveis e sintaxe do programa.
                    printf("LeitorBMP <nome do arquivo/caminho> -flags ...\n\n\n-d          Imprime dados da imagem\n-e          Tira os espaços entre os pontos quando sao impressos na tela.\n-h          Imprime esse texto.\n-o          Especifica um arquivo para escrever o ASCII\n");
                    free(flags);
                    flags = NULL;
                    return flags;
                    break;
                case 'e':
                    //-e coloca espaços entre os caracteres no output.
                    flags[1] = 1;
                    break;
                case 'd':
                    //-d imprime os dados da imagem.
                    flags[2] = 1;
                    break;
                case 's':
                    //-s imprime se tudo ceu certo ou não
                    flags[3] = 1;
                    break;
                case 'o':
                    //-o indica qual é o arquivo onde será escrito o ascii.
                    flags[4] = 1;
                    if(++i == n_arg || argv[i][0] == '-')
                    {
                        tres = flags[3];
                        *output = NULL;
                        free(flags);
                        flags = NULL;
                        printf("LeitorBMP <nome do arquivo/caminho> -flags ...\n\n\n-d          Imprime dados da imagem\n-e          Tira os espaços entre os pontos quando sao impressos na tela.\n-h          Imprime esse texto.\n-o          Especifica um arquivo para escrever o ASCII\n");
                        return flags; // tlavez muda o jeito q sai
                    }
                    *output = fopen(argv[i], "r+");
                    break;
                default:
                    printf("LeitorBMP <nome do arquivo/caminho> -flags ...\n\n\n-d          Imprime dados da imagem\n-e          Tira os espaços entre os pontos quando sao impressos na tela.\n-h          Imprime esse texto.\n-o          Especifica um arquivo para escrever o ASCII\n");
                    free(flags);
                    flags = NULL;
                    return flags;
                    break;
            }
        }
    }

    tres = flags[3];

    if (*output == NULL)
    {
        if (tres == 1)
        {
            printf("Nao foi possivel abrir o arquivo de saida\n");
            printf("Abrindo output.txt como padrao\n");
        }
        return flags;
    }

    return flags;
}
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

unsigned int bmp_header (FILE *bmp, Imagem *bemp, int flag)
{

    char bm[3];
    unsigned int size;
    unsigned int array_st;

    fread(bm, 1, 2, bmp);
    bm[3] = '\0';

    if(bm[0] == 'B' && bm[1] == 'M'){

    }else{
        printf("O arquivo nao foi aberto corretamente ou o cabecalho nao eh suportado.\n");
        exit(0);
    }

    fseek(bmp, 2, SEEK_SET);
    fread(&size, 4, 1, bmp);
    fseek(bmp, 10, SEEK_SET);
    fread(&array_st, 4, 1, bmp);

    (*bemp).tam_bytes = size;

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

    a.compressao = compressao;

    return a;
}

void bmp_read (FILE *bmp, Imagem img, unsigned char *out, int flag)
{

    if(img.b_p_px != 24){
        if(flag == 1)
        {
            printf("O programa nao cosegue ler quando eh diferente de 24 bits por pixel atualmente.\nBits por pixel: %hd\n", img.b_p_px);
        }
        
        exit(0);
    }

    int padding;

    if(img.W%4 == 0){

        padding = 0;

    }else{
        
        int mult = img.W/4 + 1;
        
        padding = (4*mult)%img.W;

    }

    //printf("Padding: %d\n", padding);

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
    int *flags;
    Imagem BMP;
    unsigned int arr_st;
    int offset;
    FILE *output = NULL;

    flags = ler_flags(&output, argc, argv);

    if (output == NULL)
    {
        output = fopen("output.txt","w");
    }

    if(flags == NULL)
    {
        return 0;
    }

    int n_flags = sizeof(flags);

    //se o comando consistir apenas de flags, o programa termina
    if(n_flags == argc - 1 && argc != 1)
    {
        printf("LeitorBMP <nome do arquivo/caminho> -flags ...\n\n\n-d          Imprime dados da imagem\n-e          Tira os espaços entre os pontos quando sao impressos na tela.\n-h          Imprime esse texto.\n-o          Especifica um arquivo para escrever o ASCII\n");
        free(flags);
        free(path);
        return 0;
    }

    if(argc == 1)
    {
        printf("Digite o nome do arquivo (se ele existir no diretorio do programa) ");
        printf("ou o caminho completo dele:\n");

        fgets(path, 64, stdin);

        path[strlen(path) - 1] = '\0';
    }
    else
    {
        free(path);
        path = argv[1];
    }

    FILE *bmp = bmpopen(path);

    if (bmp == NULL)
    {
        if(flags[3] == 1)
        {
            printf("O path nao esta correto ou o arquivo nao existe\n(ou o negocio so n abriu msm)\n");
        }
        return 0;
    }

    arr_st = bmp_header(bmp, &BMP, flags[3]);

    if(flags[3] == 1)
    {
        printf("O arquivo foi aberto corretamente\n");
    }

    fseek(bmp, 14, SEEK_SET);
    fread(&offset, sizeof(int), 1, bmp);

    {
        //arruma isso dps pra ele nao perder esses dados quando ler o DIB header
        sh comp = BMP.compressao;
        unsigned int tam = BMP.tam_bytes;

        BMP = DIB_header(bmp);

        BMP.compressao = comp;
        BMP.tam_bytes = tam;
    }

    unsigned char *im_bmp = (unsigned char *) malloc(BMP.W*BMP.H*sizeof(char));

    if (im_bmp == NULL)
    {
        printf("Sem memoria\n");
        abort();
    }

    fseek(bmp, arr_st, SEEK_SET);

    bmp_read(bmp, BMP, im_bmp, flags[3]);

    if (output != NULL)
    {

    for (int i=0; i<BMP.H; i++)
    {
    for (int j=0; j<BMP.W; j++)
    {
        fputc((int) im_bmp[i*BMP.W + j], output);

        if (flags[1] == 1)
        {
            fputc((int) ' ',output);
        }
    }
    fputc((int) '\n', output);
    }
    }
    else
    {
        if(flags[3] == 1)
        printf("Nao foi possivel abrir o arquivo de saida.\n");
    }

    if (flags[2] == 1)
    {
        printf("Tamanho (em pixels):\n%dx%d\n", BMP.W, BMP.H);
        printf("Tamanho (em bytes):\n%u\n", BMP.tam_bytes);
        printf("Bits por pixel:\n%hu\n", BMP.b_p_px);
        printf("Compressao:\n");
        
        if(BMP.compressao == 0)
        {
            printf("Sem compressao\n");
        }
        else
        {
            printf("Compressao(mas nao sei qual)\n");
        }
        printf("\n");
    }

    if(n_flags > 0 )
    {
        free(flags);
    }
    fclose(output);
    fclose(bmp);

    if(argc == 1)
    {
        free(path);
    }

    free(im_bmp);

    return 0;

}