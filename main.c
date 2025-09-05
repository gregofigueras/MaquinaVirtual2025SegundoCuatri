#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <C:\Users\marti\CLionProjects\MaquinaVirtual2025SegundoCuatri\Mnemonicos.h>
#define N 16384

void CargarVmx(char NombreArchivo[10], int Memoria[N], int Registros[32],  short int TablaSegmentos[8][2]);
void ProcesarInstrucciones(int Memoria[N], int Registros[32], short int TablaSegmentos[8][2], bool Dissasembler);
void Imprimir_Dissasembler(int Memoria[N], short int TablaSegmentos[8][2]);
void Set_Instruccion(int Instruccion, int Registros[32]);
void Mover_Ip(int Registros[32]);
void Set_Operando(int Memoria[N], int Registros[32], int *direccionFisica);
void Set_OperandoValor(int Memoria[N],int *Registro, int *direccionFisica);


int main(int argc,char * argv[]) {
    unsigned char Memoria[N];
    short int TablaSegmentos[8][2];
    int Registros[32];
    bool Dissasembler=false;
    char DireccionVmx[10];

    for (int i=0;i<argc;i++){
        if (strcmp(".vmx", strrchr(argv[i],'.'))==0){
            strcpy(DireccionVmx,argv[i]);
        }
        if (strcmp(argv[i],"-d")==0)
            Dissasembler=true;
    }
    CargarVmx(DireccionVmx,Memoria,Registros,TablaSegmentos);

    return 0;
}

void CargarVmx(char NombreArchivo[10], int Memoria[N], int Registros[32], short int TablaSegmentos[8][2]) {
    FILE *Archivo;
    int i = 0;
    Archivo = fopen(NombreArchivo, "rb");
    if (Archivo == NULL) {
        printf("Error al abrir el archivo.\n");
        return;
    }

    fseek(Archivo,0,SEEK_END);
    int TamanoArchivo= ftell(Archivo);
    unsigned char data[TamanoArchivo];
    fseek(Archivo,0,SEEK_SET);

    fclose(Archivo);
    int TamanoCs= (data[6] << 8) | data[7];
    TablaSegmentos[0][0]=0x0000;
    TablaSegmentos[0][1]=TamanoCs;
    TablaSegmentos[1][0]=TamanoCs;
    TablaSegmentos[1][1]=N-TamanoCs;



    Registros[26]=0x00000000; // CS
    Registros[3]= Registros[26]; // IP = CS
    Registros[27]=0x00010000; // DS
    int j=0;
    for (i=18; i<TamanoArchivo;i++){
        Memoria[j]=data[i];
        j++;
    }

}

void ProcesarInstrucciones(int Memoria[N], int Registros[32], short int TablaSegmentos[8][2], bool Dissasembler) {;
    int aux;
    int direccionFisica=TablaSegmentos[((Registros[26] & 0xFFFF0000)>>16)][0];
    if (Dissasembler) {
        Imprimir_Dissasembler(Memoria,TablaSegmentos);
    }

    for (;direccionFisica<TablaSegmentos[((Registros[26] & 0xFFFF0000)>>16)][1];) {
        aux=Memoria[direccionFisica];
        Set_Instruccion(aux,Registros);  // Mueve IP
        Mover_Ip(Registros);
        direccionFisica++;
        Set_Operando(Memoria,Registros,&direccionFisica);
        // Llama Mnemonicos

        switch (Registros[4] & 0x000000FF) {
            case 0:
                SYS(Memoria,Registros,TablaSegmentos);
                break;
            case 1:
                JMP(Memoria,Registros,TablaSegmentos);
                break;
            case 2:
                JZ(Memoria,Registros,TablaSegmentos);
                break;
            case 3:
                JP(Memoria,Registros,TablaSegmentos);
                break;
            case 4:
                JN(Memoria,Registros,TablaSegmentos);
                break;
            case 5:
                JNZ(Memoria,Registros,TablaSegmentos);
                break;
            case 6:
                JNP(Memoria,Registros,TablaSegmentos);
                break;
            case 7:
                JNN(Memoria,Registros,TablaSegmentos);
                break;
            case 8:
                NOT(Memoria,Registros,TablaSegmentos);
                break;
            case 15:
                STOP();
             break;
            case 16:
                MOV(Memoria,Registros,TablaSegmentos);
                break;
            case 17:
                ADD(Memoria,Registros,TablaSegmentos);
                break;
            case 18:
                SUB(Memoria,Registros,TablaSegmentos);
                break;
            case 19:
                MUL(Memoria,Registros,TablaSegmentos);
                break;
            case 20:
                DIV(Memoria,Registros,TablaSegmentos);
                break;
            case 21:
                CMP(Memoria,Registros,TablaSegmentos);
                break;
            case 22:
                SHL(Memoria,Registros,TablaSegmentos);
                break;
            case 23:
                SHR(Memoria,Registros,TablaSegmentos);
                break;
            case 24:
                SAR(Memoria,Registros,TablaSegmentos);
                break;
            case 25:
                AND(Memoria,Registros,TablaSegmentos);
                break;
            case 26:
                OR(Memoria,Registros,TablaSegmentos);
                break;
            case 27:
                XOR(Memoria,Registros,TablaSegmentos);
                break;
            case 28:
                SWAP(Memoria,Registros,TablaSegmentos);
                break;
            case 29:
                LDL(Memoria,Registros,TablaSegmentos);
                break;
            case 30:
                LDH(Memoria,Registros,TablaSegmentos);
                break;
                case 31:
                RND(Memoria,Registros,TablaSegmentos);
                break;


        }
    }

}

void Set_Instruccion(int Instruccion, int Registros[32]){
    Registros[4]= Instruccion & 0x1F; // Carga el OPC

    Instruccion = Instruccion >> 4;   //Carga los tipos de los OP1 y OP2
    if (Instruccion % 2) { // 2 Operandos
        Registros[5]= (Instruccion & 0x3) << 24;
        Registros[6]= (Instruccion & 0xC) << 24;

        // MOVER IP
        Registros[3]+= 1 + ((Registros[5] & 0xFF000000)>>24) + ((Registros[6] & 0xFF000000)>>24);
    }
    else if (Instruccion != 0) { // 1 Operando
        Registros[5]= (Instruccion & 0xC) <<24;
        // MOVER IP
        Registros[3]+= 1 + ((Registros[5] & 0xFF000000)>>24);
    }

}


void Set_Operando(int Memoria[N], int Registros[32], int *direccionFisica){

    if (Registros[4] >= 16) {  //2 operandos
       Set_OperandoValor(Memoria,&Registros[6],direccionFisica);
       Set_OperandoValor(Memoria,&Registros[5],direccionFisica);
    }
    else {
        if (Registros[4] <= 8) {  //1 operando
            Set_OperandoValor(Memoria,&Registros[5],direccionFisica);
        }
        else
            if (Registros[4] == 15) { // 0 Operandos
                //No hace nada
            }
    }
}

void Set_OperandoValor(int Memoria[N],int *Registro, int *direccionFisica) {
    if (*Registro == 0x01000000) // Registros
        *Registro= *Registro | Memoria[*direccionFisica];
    else if (*Registro==0x02000000) // Inmediato
        *Registro= *Registro | ((Memoria[*direccionFisica]<<8) | Memoria[++(*direccionFisica)]);
    else if (*Registro==0x03000000) // Memoria
        *Registro= *Registro | ((Memoria[*direccionFisica]<<16) | (Memoria[++(*direccionFisica)]<<8) | Memoria[++(*direccionFisica)]);
}

