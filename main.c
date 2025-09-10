#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <C:\Users\marti\CLionProjects\MaquinaVirtual2025SegundoCuatri\Mnemonicos.h>
#define N 16384

void CargarVmx(char NombreArchivo[10], unsigned char Memoria[N], int Registros[32],  short int TablaSegmentos[8][2]);
void ProcesarInstrucciones(unsigned char Memoria[N], int Registros[32], short int TablaSegmentos[8][2], bool Dissasembler);
void Imprimir_Dissasembler(unsigned char Memoria[N], short int TablaSegmentos[8][2]);
void Set_Instruccion(int Instruccion, int Registros[32]);
void Mover_Ip(int Registros[32]);
void Set_Operando(unsigned char Memoria[N], int Registros[32], int *direccionFisica);
void Set_OperandoValor(unsigned char Memoria[N],int *Registro, int *direccionFisica);


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
    if (Dissasembler)
        Imprimir_Dissasembler(Memoria,TablaSegmentos);

    return 0;
}

void CargarVmx(char NombreArchivo[10], unsigned char Memoria[N], int Registros[32], short int TablaSegmentos[8][2]) {
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

void ProcesarInstrucciones(unsigned char Memoria[N], int Registros[32], short int TablaSegmentos[8][2], bool Dissasembler) {;
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
            default:
                STOP(-5);
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


void Set_Operando(unsigned char Memoria[N], int Registros[32], int *direccionFisica){

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

void Set_OperandoValor(unsigned char Memoria[N],int *Registro, int *direccionFisica) {
    if (*Registro == 0x01000000) // Registros
        *Registro= *Registro | Memoria[*direccionFisica];
    else if (*Registro==0x02000000) // Inmediato
        *Registro= *Registro | ((Memoria[*direccionFisica]<<8) | Memoria[++(*direccionFisica)]);
    else if (*Registro==0x03000000) // Memoria
        *Registro= *Registro | ((Memoria[*direccionFisica]<<16) | (Memoria[++(*direccionFisica)]<<8) | Memoria[++(*direccionFisica)]);
}

void Imprimir_Dissasembler(unsigned char Memoria[N], short int TablaSegmentos[8][2]) {
    char TagMnemonico[4]; char Operando[10]; char Operando2[10];
    int PosicionFisica=0, PosicionInstruccion,aux;
    long int acumulador=0;
    bool op1=false,op2=false;
    while (PosicionFisica<TablaSegmentos[1][0]) {
        PosicionInstruccion=PosicionFisica;
        acumulador=Memoria[PosicionFisica];
        op1=op2=false;
        switch (Memoria[PosicionFisica] & 0x1F) {
            case 0:
                strcpy(TagMnemonico,"SYS");
                break;
            case 1:
                strcpy(TagMnemonico,"JMP");
                break;
            case 2:
                strcpy(TagMnemonico,"JZ");
                break;
            case 3:
                strcpy(TagMnemonico,"JP");
                break;
            case 4:
                strcpy(TagMnemonico,"JN");
                break;
            case 5:
                strcpy(TagMnemonico,"JNZ");

                break;
            case 6:
                strcpy(TagMnemonico,"JNP");
                break;
            case 7:
                strcpy(TagMnemonico,"JNN");
                break;
            case 8:
                strcpy(TagMnemonico,"NOT");
                break;
            case 15:
                strcpy(TagMnemonico,"STOP");
                break;
            case 16:
                strcpy(TagMnemonico,"MOV");
                break;
            case 17:
                strcpy(TagMnemonico,"ADD");
                break;
            case 18:
                strcpy(TagMnemonico,"SUB");
                break;
            case 19:
                strcpy(TagMnemonico,"MUL");
                break;
            case 20:
                strcpy(TagMnemonico,"DIV");
                break;
            case 21:
                strcpy(TagMnemonico,"CMP");
                break;
            case 22:
                strcpy(TagMnemonico,"SHL");
                break;
            case 23:
                strcpy(TagMnemonico,"SHR");
                break;
            case 24:
                strcpy(TagMnemonico,"SAR");
                break;
            case 25:
                strcpy(TagMnemonico,"AND");
                break;
            case 26:
                strcpy(TagMnemonico,"OR");
                break;
            case 27:
                strcpy(TagMnemonico,"XOR");
                break;
            case 28:
                strcpy(TagMnemonico,"SWAP");
                break;
            case 29:
                strcpy(TagMnemonico,"LDL");
                break;
            case 30:
                strcpy(TagMnemonico,"LDH");
                break;
            case 31:
                strcpy(TagMnemonico,"RND");
                break;
            default:
                STOP(-5);
                break;
        }
        if ((Memoria[PosicionFisica] & 0xF0) % 2){ // Es de 2 Operando
            op2=true;
            if ((Memoria[PosicionFisica] & 0xC0)>>4 == 12) {  // 3 bytes
                PosicionFisica++;
                switch (Memoria[PosicionFisica] & 0x1F) {
                    case 1:
                        strcpy(Operando2,"[LAR");
                        break;
                    case 2:
                        strcpy(Operando2,"[MAR");
                        break;
                    case 3:
                        strcpy(Operando2,"[MBR");
                        break;
                    case 4:
                        strcpy(Operando2,"[IP");
                        break;
                    case 5:
                        strcpy(Operando2,"[OP1");
                        break;
                    case 6:
                        strcpy(Operando2,"[OP2");
                        break;
                    case 10:
                        strcpy(Operando2,"[EAX");
                        break;
                    case 11:
                        strcpy(Operando2,"[EBX");
                        break;
                    case 12:
                        strcpy(Operando2,"[ECX");
                        break;
                    case 13:
                        strcpy(Operando2,"[EDX");
                        break;
                    case 14:
                        strcpy(Operando2,"[EEX");
                        break;
                    case 15:
                        strcpy(Operando2,"[EFX");
                        break;
                    case 16:
                        strcpy(Operando2,"[AC");
                        break;
                    case 17:
                        strcpy(Operando2,"[CC");
                        break;
                    case 26:
                        strcpy(Operando2,"[CS");
                        break;
                    case 27:
                        strcpy(Operando2,"[DS");
                        break;
                }
                acumulador=acumulador<<8 | Memoria[PosicionFisica];
                PosicionFisica++;
                aux=Memoria[PosicionFisica] <<8;
                acumulador= acumulador<<8 | Memoria[PosicionFisica];
                PosicionFisica++;
                aux= aux | Memoria[PosicionFisica];
                acumulador= acumulador<<8 | Memoria[PosicionFisica];
                if (aux) {
                    char num[10],mas='+',cierre=']';
                    sprintf(num,"%s%d%s",mas,aux,cierre);
                    strcat(Operando2, num);
                }
                else
                    strcat(Operando2,"]");



            } else if ((Memoria[PosicionFisica] & 0xC0)>>4 == 8) { // 2 bytes inmediato
                PosicionFisica++;
                aux=Memoria[PosicionFisica] <<8;
                acumulador= acumulador<<8 | Memoria[PosicionFisica];
                PosicionFisica++;
                aux= aux | Memoria[PosicionFisica];
                acumulador= acumulador<<8 | Memoria[PosicionFisica];
                sprintf(Operando2,"%d",aux);

            } else if ((Memoria[PosicionFisica] & 0xC0)>>4 == 4) { // 1 byte registro
                PosicionFisica++;
                switch (Memoria[PosicionFisica] & 0x1F) {
                    case 1:
                        strcpy(Operando2,"[LAR]");
                        break;
                    case 2:
                        strcpy(Operando2,"[MAR]");
                        break;
                    case 3:
                        strcpy(Operando2,"[MBR]");
                        break;
                    case 4:
                        strcpy(Operando2,"[IP]");
                        break;
                    case 5:
                        strcpy(Operando2,"[OP1]");
                        break;
                    case 6:
                        strcpy(Operando2,"[OP2]");
                        break;
                    case 10:
                        strcpy(Operando2,"[EAX]");
                        break;
                    case 11:
                        strcpy(Operando2,"[EBX]");
                        break;
                    case 12:
                        strcpy(Operando2,"[ECX]");
                        break;
                    case 13:
                        strcpy(Operando2,"[EDX]");
                        break;
                    case 14:
                        strcpy(Operando2,"[EEX]");
                        break;
                    case 15:
                        strcpy(Operando2,"[EFX]");
                        break;
                    case 16:
                        strcpy(Operando2,"[AC]");
                        break;
                    case 17:
                        strcpy(Operando2,"[CC]");
                        break;
                    case 26:
                        strcpy(Operando2,"[CS]");
                        break;
                    case 27:
                        strcpy(Operando2,"[DS]");
                        break;
                }
                acumulador=acumulador<<8 | Memoria[PosicionFisica];
            }
        }
        else if ((Memoria[PosicionFisica] & 0x0F) !=0) { // Es de 1 Operandos
            op1=true;
            if ((Memoria[PosicionFisica] & 0xC0)>>4 == 12) {  // 3 bytes
                PosicionFisica++;
                switch (Memoria[PosicionFisica] & 0x1F) {
                    case 1:
                        strcpy(Operando,"[LAR");
                        break;
                    case 2:
                        strcpy(Operando,"[MAR");
                        break;
                    case 3:
                        strcpy(Operando,"[MBR");
                        break;
                    case 4:
                        strcpy(Operando,"[IP");
                        break;
                    case 5:
                        strcpy(Operando,"[OP1");
                        break;
                    case 6:
                        strcpy(Operando,"[OP2");
                        break;
                    case 10:
                        strcpy(Operando,"[EAX");
                        break;
                    case 11:
                        strcpy(Operando,"[EBX");
                        break;
                    case 12:
                        strcpy(Operando,"[ECX");
                        break;
                    case 13:
                        strcpy(Operando,"[EDX");
                        break;
                    case 14:
                        strcpy(Operando,"[EEX");
                        break;
                    case 15:
                        strcpy(Operando,"[EFX");
                        break;
                    case 16:
                        strcpy(Operando,"[AC");
                        break;
                    case 17:
                        strcpy(Operando,"[CC");
                        break;
                    case 26:
                        strcpy(Operando,"[CS");
                        break;
                    case 27:
                        strcpy(Operando,"[DS");
                        break;
                }
                acumulador=acumulador<<8 | Memoria[PosicionFisica];
                PosicionFisica++;
                aux=Memoria[PosicionFisica] <<8;
                acumulador= acumulador<<8 | Memoria[PosicionFisica];
                PosicionFisica++;
                aux= aux | Memoria[PosicionFisica];
                acumulador= acumulador<<8 | Memoria[PosicionFisica];
                if (aux) {
                    char num[10],mas='+',cierre=']';
                    sprintf(num,"%s%d%s",mas,aux,cierre);
                    strcat(Operando, num);
                }
                else
                    strcat(Operando,"]");



            } else if ((Memoria[PosicionFisica] & 0xC0)>>4 == 8) { // 2 bytes inmediato
                PosicionFisica++;
                aux=Memoria[PosicionFisica] <<8;
                acumulador= acumulador<<8 | Memoria[PosicionFisica];
                PosicionFisica++;
                aux= aux | Memoria[PosicionFisica];
                acumulador= acumulador<<8 | Memoria[PosicionFisica];
                sprintf(Operando,"%d",aux);

            } else if ((Memoria[PosicionFisica] & 0xC0)>>4 == 4) { // 1 byte registro
                PosicionFisica++;
                switch (Memoria[PosicionFisica] & 0x1F) {
                    case 1:
                        strcpy(Operando,"[LAR]");
                        break;
                    case 2:
                        strcpy(Operando,"[MAR]");
                        break;
                    case 3:
                        strcpy(Operando,"[MBR]");
                        break;
                    case 4:
                        strcpy(Operando,"[IP]");
                        break;
                    case 5:
                        strcpy(Operando,"[OP1]");
                        break;
                    case 6:
                        strcpy(Operando,"[OP2]");
                        break;
                    case 10:
                        strcpy(Operando,"[EAX]");
                        break;
                    case 11:
                        strcpy(Operando,"[EBX]");
                        break;
                    case 12:
                        strcpy(Operando,"[ECX]");
                        break;
                    case 13:
                        strcpy(Operando,"[EDX]");
                        break;
                    case 14:
                        strcpy(Operando,"[EEX]");
                        break;
                    case 15:
                        strcpy(Operando,"[EFX]");
                        break;
                    case 16:
                        strcpy(Operando,"[AC]");
                        break;
                    case 17:
                        strcpy(Operando,"[CC]");
                        break;
                    case 26:
                        strcpy(Operando,"[CS]");
                        break;
                    case 27:
                        strcpy(Operando,"[DS]");
                        break;
                }
                acumulador=acumulador<<8 | Memoria[PosicionFisica];
            }
        }
    else { // Es de 0 Operandos
        printf("[%04X] %12X | %s\n",PosicionInstruccion,acumulador,TagMnemonico);
        PosicionFisica++;

    }
    if (op1){
        printf("[%04X] %12X | %s   %s\n",PosicionInstruccion,acumulador,TagMnemonico,Operando);
    }
    else if (op2) {
        printf("[%04X] %12X | %s   %s, %s\n",PosicionInstruccion,acumulador,TagMnemonico,Operando, Operando2);
    }
}

}