#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include "Mnemonicos.h"
#define N 16384

void CargarVmx(char NombreArchivo[256], unsigned char Memoria[N], int Registros[32],  short int TablaSegmentos[8][2], int TamMem);
void CargaVMI(char NombreArchivo[256], unsigned char Memoria[N], int Registros[32], short int TablaSegmentos[8][2], int TamMem);
void ProcesarInstrucciones(unsigned char Memoria[N], int Registros[32], short int TablaSegmentos[8][2], char DireccionVmi[256],short int TamMemoria);
void Imprimir_Dissasembler(unsigned char Memoria[N], short int TablaSegmentos[8][2], int EntryPoint,int Registros[32]);
void Set_Instruccion(int Instruccion, int Registros[32]);
void Set_Operando(unsigned char Memoria[N], int Registros[32], int *direccionFisica);
void Set_OperandoValor(unsigned char Memoria[N],int *Registro, int *direccionFisica);
void CargaPila (unsigned char Memoria[N], int Registros[32], short int TablaSegmentos[8][2], int CuentaPalabras);
void Set_Operando_Dissasembler(char Operando[11],unsigned char Memoria[N], int *PosicionFisica,long int *acumulador, int byte);


int main(int argc,char * argv[]) {
    int i;
    int argvP[100];
    int cuentaPalabra=0;
    int cuentamemoria=0;
    int TamMemoria=N;
    unsigned char Memoria[N];
    short int TablaSegmentos[8][2];
    int Registros[32];
    bool Dissasembler=false, VMX=false, Parametros=false;
    char DireccionVmx[256], DireccionVmi[256];
    strcpy(DireccionVmx,"");
    strcpy(DireccionVmi,"");
    for ( i=0; i< 32;i++) //Inicializa Registros
        Registros[i]=-1;
    for (i = 1; (i < argc) && (!Parametros); i++) {
        if (strstr(argv[i], ".vmx")) {
            strcpy(DireccionVmx, argv[i]);
            VMX=true;
        }
        else if (strstr(argv[i], ".vmi")) {
            strcpy(DireccionVmi, argv[i]);
        }
        else if (strncmp(argv[i], "m=", 2) == 0) {
            TamMemoria = atoi(argv[i] + 2);;
        }
        else if (strcmp(argv[i], "-d") == 0) { //Dissasembler
            Dissasembler = true;
        }
        else if (strcmp(argv[i],"-p")==0) {
            //Parametros
            Parametros = true;
        }
    }
    for (;i<argc;i++) {
        char Palabra[200];
        argvP[cuentaPalabra]=cuentamemoria;
        strcpy(Palabra, argv[i]);
        for (int j=0 ;strlen(Palabra) >= j;j++) {
            Memoria[cuentamemoria] = Palabra[j];
            cuentamemoria++;
        }
        cuentaPalabra++;
    }
    for (i=0;i<cuentaPalabra;i++) {
            Memoria[cuentamemoria]= argvP[i] & 0xFF000000;
            Memoria[cuentamemoria+1]= argvP[i] & 0x00FF0000;
            Memoria[cuentamemoria+2]= argvP[i] & 0x0000FF00;
            Memoria[cuentamemoria+3]= argvP[i] & 0x000000FF;
            cuentamemoria+=4;
    }
    if (Parametros){
    TablaSegmentos[0][0]=0x0000;
    TablaSegmentos[0][1]=cuentamemoria;
    }
    if (VMX) {
        CargarVmx(DireccionVmx,Memoria,Registros,TablaSegmentos,TamMemoria);
        CargaPila(Memoria,Registros,TablaSegmentos,cuentaPalabra);
    }
    else
        CargaVMI(DireccionVmi, Memoria,Registros,TablaSegmentos,TamMemoria);
    if (Dissasembler)
        Imprimir_Dissasembler(Memoria,TablaSegmentos,Registros[3],Registros);
    ProcesarInstrucciones(Memoria,Registros,TablaSegmentos, DireccionVmi,TamMemoria);
    return 0;
}

void CargarVmx(char NombreArchivo[256], unsigned char Memoria[N], int Registros[32], short int TablaSegmentos[8][2], int TamMem) {
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
    fread(data, 1, TamanoArchivo, Archivo);
    fclose(Archivo);
    if (data[5]==1) {
        int TamanoCs= (data[6] << 8) | data[7];
        TablaSegmentos[0][0]=0x0000;
        TablaSegmentos[0][1]=TamanoCs;
        TablaSegmentos[1][0]=TamanoCs;
        TablaSegmentos[1][1]=N-TamanoCs;



        Registros[26]=0x00000000; // CS
        Registros[3]= Registros[26]; // IP = CS
        Registros[27]=0x00010000; // DS
        int j=0;
        for (i=8; i<TamanoArchivo;i++) {
            Memoria[j]=data[i];
            j++;
        }
    }else if (data[5]==2) {
        int i=0;
        int TamanoCs= (data[6] << 8) | data[7];
        int TamanoDs= (data[8] << 8) | data[9];
        int TamanoEs= (data[10] << 8) | data[11];
        int TamanoSS= (data[12] << 8) | data[13];
        int TamanoConstS= (data[14] << 8) | data[15];
        int OffsetEntry= (data[16] << 8) | data[17];
        if ((TamanoCs+TamanoDs+TamanoEs+TamanoSS+TamanoConstS)> TamMem) {
            printf("Memoria insuficiente");
            exit(-5);
        }
        int TamanoPS = 0;
        if (TablaSegmentos[0][1]) {
            TamanoPS= TablaSegmentos[0][1];
            Registros[31]= i<<16;
            i++;
        }
        if (TamanoConstS) {
            Registros[30]= i<<16;
            if (i==0) {
                TablaSegmentos[i][0]=0;
                TablaSegmentos[i][1]=TamanoConstS;
            }
            else {
                TablaSegmentos[i][0]= TablaSegmentos[i-1][0]+ TablaSegmentos[i-1][1];
                TablaSegmentos[i][1]= TamanoConstS;
            }
            int j=TamanoArchivo-TamanoConstS;
            for (int i=TamanoPS; i<TamanoConstS+TamanoPS;i++) {
                Memoria[i]= data[j];
                j++;
            }
            i++;
        }
        if (TamanoCs) {
            if (OffsetEntry)
                Registros[3]= i<<16 | (OffsetEntry & 0x0000FFFF);
            Registros[26]= i<<16;
            if (i==0) {
                TablaSegmentos[i][0]=0;
                TablaSegmentos[i][1]=TamanoCs;
            }
            else {
                TablaSegmentos[i][0]= TablaSegmentos[i-1][0]+ TablaSegmentos[i-1][1];
                TablaSegmentos[i][1]= TamanoCs;
            }
            i++;
        }
        if (TamanoDs) {
            Registros[27] = i<<16;
            TablaSegmentos[i][0]= TablaSegmentos[i-1][0]+ TablaSegmentos[i-1][1];
            TablaSegmentos[i][1]= TamanoDs;
            i++;
        }
        if (TamanoEs) {
            Registros[28]= i<<16;
            TablaSegmentos[i][0]= TablaSegmentos[i-1][0]+ TablaSegmentos[i-1][1];
            TablaSegmentos[i][1]= TamanoEs;
            i++;
        }
        if (TamanoSS) {
            Registros[29]= i<<16;
            TablaSegmentos[i][0]= TablaSegmentos[i-1][0]+ TablaSegmentos[i-1][1];
            TablaSegmentos[i][1]= TamanoSS;
            Registros[7]= Registros[29] | TamanoSS; // SP = SS:limite
        }
        int j=TablaSegmentos[Registros[26]>>16][0];
        for (i=18; i<TamanoArchivo-TamanoConstS;i++) {
            Memoria[j]=data[i];
            j++;
        }
    }
}


void CargaVMI(char NombreArchivo[256], unsigned char Memoria[N], int Registros[32], short int TablaSegmentos[8][2], int TamMem){
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
    fread(data, 1, TamanoArchivo, Archivo);
    int TamanoMemoria= ((data[6] <<8) + data[7]) *1024;
    fclose(Archivo);
    if ((TamanoMemoria)> TamMem)
        printf("Memoria insuficiente");
    int k=8;
    for (i=0; i<32;i++) { // Registros
        Registros[i]=0;
        for (int j=3; j>=0; j--){
            Registros[i]=Registros[i] | data[k] << j*8;
            k++;
        }
    }
    for (i=0; i<8; i++) { //Tabla Segmentos
        for (int j=0; j<2; j++) {
            for (int q=1; q>=0; q--) {
                TablaSegmentos[i][j]= TablaSegmentos[i][j] | data[k] << q*8;
                k++;
            }
        }
    }

    for (i=0; i<TamanoMemoria; i++) { //Carga Memoria
        Memoria[i]=data[k];
        k++;
    }

}

void ProcesarInstrucciones(unsigned char Memoria[N], int Registros[32], short int TablaSegmentos[8][2], char DireccionVmi[256],short int TamMemoria) {;
    int aux,breakpoint=0;
    int direccionFisica=TablaSegmentos[((Registros[3] & 0xFFFF0000)>>16)][0] + (Registros[3] & 0x0000FFFF);
    bool jump,ret;
    for (;direccionFisica<TablaSegmentos[((Registros[26] & 0xFFFF0000)>>16)][1]+TablaSegmentos[((Registros[26] & 0xFFFF0000)>>16)][0];) {
        jump = false; // Salto
        ret=false; //Ret
        aux=Memoria[direccionFisica];
        Set_Instruccion(aux,Registros);  // Mueve IP
        direccionFisica++;
        Set_Operando(Memoria,Registros,&direccionFisica);
        // Llama Mnemonicos

        switch (Registros[4] & 0x000000FF) {
            case 0:
                SYS(Memoria,Registros,TablaSegmentos,DireccionVmi,TamMemoria,&breakpoint);
                break;
            case 1:
                jump = JMP(Memoria,Registros,TablaSegmentos);
                break;
            case 2:
                jump = JZ(Memoria,Registros,TablaSegmentos);
                break;
            case 3:
                jump = JP(Memoria,Registros,TablaSegmentos);
                break;
            case 4:
                jump = JN(Memoria,Registros,TablaSegmentos);
                break;
            case 5:
                jump = JNZ(Memoria,Registros,TablaSegmentos);
                break;
            case 6:
                jump = JNP(Memoria,Registros,TablaSegmentos);
                break;
            case 7:
                jump = JNN(Memoria,Registros,TablaSegmentos);
                break;
            case 8:
                NOT(Memoria,Registros,TablaSegmentos);
                break;
            case 11:
                PUSH(Memoria,Registros,TablaSegmentos);
                break;
            case 12:
                POP(Memoria,Registros,TablaSegmentos);
                break;
            case 13:
                CALL(Memoria,Registros,TablaSegmentos);
                jump=true;
                break;
            case 14:
                RET(Memoria,Registros,TablaSegmentos);
                ret=true;
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
                exit(-5);
                break;

        }
        if (jump) // salto
            direccionFisica = TablaSegmentos[((Registros[3] & 0xFFFF0000)>>16)][0] + (Registros[3] & 0x0000FFFF);
        else if (ret) {
            direccionFisica = TablaSegmentos[((Registros[3] & 0xFFFF0000)>>16)][0] + (Registros[3] & 0x0000FFFF);
        }
        else
            direccionFisica++;
        if (breakpoint==2) {
            breakpoint=0;
            SYS(Memoria,Registros,TablaSegmentos,DireccionVmi,TamMemoria,&breakpoint);
        }
        else if (breakpoint==1)
            breakpoint==2;
    }
    printf("");
}

void Set_Instruccion(int Instruccion, int Registros[32]){
    Registros[4]= Instruccion & 0x1F; // Carga el OPC

    Instruccion = Instruccion >> 4;   //Carga los tipos de los OP1 y OP2
    if (Instruccion % 2) { // 2 Operandos
        Registros[5]= (Instruccion & 0x3) << 24;
        Registros[6]= ((Instruccion & 0xC)>>2) << 24;

        // MOVER IP
        Registros[3]+= 1 + ((Registros[5] & 0xFF000000)>>24) + ((Registros[6] & 0xFF000000)>>24);
    }
    else if (Instruccion != 0) { // 1 Operando
        Registros[5]= ((Instruccion & 0xC)>>2) <<24;
        Registros[6]=0;
        // MOVER IP
        Registros[3]+= 1 + ((Registros[5] & 0xFF000000)>>24);
    }

}


void Set_Operando(unsigned char Memoria[N], int Registros[32], int *direccionFisica){

    if (Registros[4] >= 16) {  //2 operandos
       Set_OperandoValor(Memoria,&Registros[6],direccionFisica);
        (*direccionFisica)++;
       Set_OperandoValor(Memoria,&Registros[5],direccionFisica);
    }
    else {
        if (Registros[4]<14) {  //1 operando
            Set_OperandoValor(Memoria,&Registros[5],direccionFisica);
        }
        else
            if (Registros[4] >= 14) { // 0 Operandos
                //No hace nada
            }
    }
}

void Set_OperandoValor(unsigned char Memoria[N],int *Registro, int *direccionFisica) {
    if (*Registro == 0x01000000) // Registros
        (*Registro)= *Registro | Memoria[*direccionFisica];
    else if (*Registro==0x02000000) // Inmediato
        (*Registro)= *Registro | ((Memoria[*direccionFisica]<<8) | Memoria[++(*direccionFisica)]);
    else if (*Registro==0x03000000) // Memoria
        (*Registro)= *Registro | ((Memoria[*direccionFisica]<<16) | (Memoria[++(*direccionFisica)]<<8) | Memoria[++(*direccionFisica)]);
}


void Set_Operando_Dissasembler(char Operando[11],unsigned char Memoria[N], int *PosicionFisica,long int *acumulador, int byte) {
    short int aux;
    if (byte==3) {  // 3 bytes
        (*PosicionFisica)++;
        switch ((Memoria[*PosicionFisica] & 0xC0) >> 6) {
            case 0:
                strcpy(Operando,"l");
                break;
            case 2:
                strcpy(Operando,"w");
                break;
            case 3:
                strcpy(Operando,"b");
                break;
        }
        switch (Memoria[*PosicionFisica] & 0x1F) {
            case 1:
                strcat(Operando, "[LAR");
                break;
            case 2:
                strcat(Operando, "[MAR");
                break;
            case 3:
                strcat(Operando, "[MBR");
                break;
            case 4:
                strcat(Operando, "[IP");
                break;
            case 5:
                strcat(Operando, "[OP1");
                break;
            case 6:
                strcat(Operando, "[OP2");
                break;
            case 7:
                strcat(Operando, "[SP");
                break;
            case 8:
                strcat(Operando, "[BP");
                break;
            case 10:
                strcat(Operando, "[EAX");
                break;
            case 11:
                strcat(Operando, "[EBX");
                break;
            case 12:
                strcat(Operando, "[ECX");
                break;
            case 13:
                strcat(Operando, "[EDX");
                break;
            case 14:
                strcat(Operando, "[EEX");
                break;
            case 15:
                strcat(Operando, "[EFX");
                break;
            case 16:
                strcat(Operando, "[AC");
                break;
            case 17:
                strcat(Operando, "[CC");
                break;
            case 26:
                strcat(Operando, "[CS");
                break;
            case 27:
                strcat(Operando, "[DS");
                break;
            case 28:
                strcat(Operando, "[ES");
                break;
            case 29:
                strcat(Operando, "[SS");
                break;
            case 30:
                strcat(Operando, "[KS");
                break;
            case 31:
                strcat(Operando, "[PS");
                break;
            default:
                break;
        }
                *acumulador=(*acumulador)<<8 | Memoria[*PosicionFisica];
                (*PosicionFisica)++;
                aux=Memoria[*PosicionFisica] <<8;
                *acumulador= (*acumulador)<<8 | Memoria[*PosicionFisica];
                (*PosicionFisica)++;
                aux= aux | Memoria[*PosicionFisica];
                *acumulador= (*acumulador)<<8 | Memoria[*PosicionFisica];
        char num[10],mas='+',cierre=']';
        if (strcmp(Operando,"[")==0)
            sprintf(num,"%d%c",aux,cierre);
        else {
            if (aux)
                sprintf(num,"%c%d%c",mas,aux,cierre);
            else
                sprintf(num,"%c",cierre);
        }
        strcat(Operando, num);



            } else if (byte==2) { // 2 bytes inmediato
                (*PosicionFisica)++;
                aux=Memoria[*PosicionFisica] <<8;
                (*acumulador)= (*acumulador)<<8 | Memoria[*PosicionFisica];
                (*PosicionFisica)++;
                aux= aux | Memoria[*PosicionFisica];
                (*acumulador)= (*acumulador)<<8 | Memoria[*PosicionFisica];
                sprintf(Operando,"%d",aux);

            } else if (byte==1) { // 1 byte registro
                (*PosicionFisica)++;
                switch (Memoria[*PosicionFisica] & 0x1F) {
                    case 1:
                        strcpy(Operando,"LAR");
                        break;
                    case 2:
                        strcpy(Operando,"MAR");
                        break;
                    case 3:
                        strcpy(Operando,"MBR");
                        break;
                    case 4:
                        strcpy(Operando,"IP");
                        break;
                    case 5:
                        strcpy(Operando,"OP1");
                        break;
                    case 6:
                        strcpy(Operando,"OP2");
                        break;
                    case 7:
                        strcpy(Operando,"SP");
                        break;
                    case 8:
                        strcpy(Operando,"BP");
                        break;
                    case 10:
                        if ((Memoria[*PosicionFisica] >> 6 ) == 3)
                            strcpy(Operando,"AX");
                        else
                            if ((Memoria[*PosicionFisica] >> 6) == 2)
                            strcpy(Operando,"AH");
                            else
                                if ((Memoria[*PosicionFisica] >> 6 ) == 1)
                                    strcpy(Operando,"AL");
                                else
                                    strcpy(Operando,"EAX");
                        break;
                    case 11:
                        if ((Memoria[*PosicionFisica] >> 6 ) == 3)
                            strcpy(Operando,"BX");
                        else
                            if ((Memoria[*PosicionFisica] >> 6) == 2)
                                strcpy(Operando,"BH");
                            else
                                if ((Memoria[*PosicionFisica] >> 6 ) == 1)
                                    strcpy(Operando,"BL");
                                else
                                    strcpy(Operando,"EBX");
                        break;
                    case 12:
                        if ((Memoria[*PosicionFisica] >> 6 ) == 3)
                            strcpy(Operando,"CX");
                        else
                            if ((Memoria[*PosicionFisica] >> 6) == 2)
                                strcpy(Operando,"CH");
                            else
                                if ((Memoria[*PosicionFisica] >> 6 ) == 1)
                                    strcpy(Operando,"CL");
                                else
                                    strcpy(Operando,"ECX");
                        break;
                    case 13:
                        if ((Memoria[*PosicionFisica] >> 6 ) == 3)
                            strcpy(Operando,"DX");
                        else
                            if ((Memoria[*PosicionFisica] >> 6) == 2)
                                strcpy(Operando,"DH");
                            else
                                if ((Memoria[*PosicionFisica] >> 6 ) == 1)
                                    strcpy(Operando,"DL");
                                else
                                    strcpy(Operando,"EDX");
                        break;
                    case 14:
                        if ((Memoria[*PosicionFisica] >> 6 ) == 3)
                            strcpy(Operando,"EX");
                        else
                            if ((Memoria[*PosicionFisica] >> 6) == 2)
                                strcpy(Operando,"EH");
                            else
                                if ((Memoria[*PosicionFisica] >> 6 ) == 1)
                                    strcpy(Operando,"EL");
                                else
                                    strcpy(Operando,"EEX");
                        break;
                    case 15:
                        if ((Memoria[*PosicionFisica] >> 6 ) == 3)
                            strcpy(Operando,"FX");
                        else
                            if ((Memoria[*PosicionFisica] >> 6 ) == 2)
                                strcpy(Operando,"FH");
                            else
                                if ((Memoria[*PosicionFisica] >> 6 ) == 1)
                                    strcpy(Operando,"FL");
                                else
                                    strcpy(Operando,"EFX");
                        break;
                    case 16:
                        strcpy(Operando,"AC");
                        break;
                    case 17:
                        strcpy(Operando,"CC");
                        break;
                    case 26:
                        strcpy(Operando,"CS");
                        break;
                    case 27:
                        strcpy(Operando,"DS");
                        break;
                    case 28:
                        strcpy(Operando,"ES");
                        break;
                    case 29:
                        strcpy(Operando,"DS");
                        break;
                    case 30:
                        strcpy(Operando,"KS");
                        break;
                    case 31:
                        strcpy(Operando,"PS");
                        break;

                }
                (*acumulador)=(*acumulador)<<8 | Memoria[(*PosicionFisica)];
            }
}



void Imprimir_Dissasembler(unsigned char Memoria[N], short int TablaSegmentos[8][2], int EntryPoint, int Registros[32]) {

    char TagMnemonico[6]; char Operando[11]; char Operando2[11];
    int PosicionFisica, PosicionInstruccion,CuentaCaracteres;
    long int acumulador=0;
    int PosicionByte;
    bool instruccion_invalida = false;
    bool op1=false,op2=false,op0=false;
    PosicionFisica= TablaSegmentos[Registros[26]>>16][0];
    unsigned char AcumulaCaracteres[256];
    // Imprimir Caracteres Constantes
    if (Registros[30]!=-1) {
        for (int i=TablaSegmentos[(Registros[30] & 0xFFFF0000)>>16][0]; i< TablaSegmentos[(Registros[30]& 0xFFFF0000)>>16][1];i++) {
            memset(AcumulaCaracteres, 0, sizeof AcumulaCaracteres);
            CuentaCaracteres=0;
            acumulador=0;
            PosicionInstruccion=i;
            while (Memoria[i]!='\0' && i< TablaSegmentos[(Registros[30]& 0xFFFF0000)>>16][1]) {
                if (Memoria[i]>=32 && Memoria[i]<=126)
                    AcumulaCaracteres[CuentaCaracteres]=Memoria[i];
                else
                    AcumulaCaracteres[CuentaCaracteres] = '.';

                acumulador=(acumulador<<8) | Memoria[i];
                CuentaCaracteres++;
                i++;
            }
            if (CuentaCaracteres<7)
                printf("[%04X] %014X | \"%s\" \n",PosicionInstruccion,acumulador,AcumulaCaracteres);
            else
                printf("[%04X] %012X.. | \"%s\" \n",PosicionInstruccion,acumulador,AcumulaCaracteres);
        }
    }
    // Imprimir Instrucciones
    while (PosicionFisica<TablaSegmentos[(Registros[26] & 0xFFFF0000)>>16][1]+TablaSegmentos[(Registros[26] & 0xFFFF0000)>>16][0]) {
        if (PosicionFisica==((EntryPoint & 0x0000FFFF) + TablaSegmentos[(Registros[26] & 0xFFFF0000)>>16][0]))
            printf(">");
        PosicionInstruccion=PosicionFisica;
        acumulador=Memoria[PosicionFisica];
        op0=false;
        op1=false;
        op2=false;
        switch (Memoria[PosicionFisica] & 0x1F) {
            case 0: strcpy(TagMnemonico,"SYS"); break;
            case 1: strcpy(TagMnemonico,"JMP"); break;
            case 2: strcpy(TagMnemonico,"JZ"); break;
            case 3: strcpy(TagMnemonico,"JP"); break;
            case 4: strcpy(TagMnemonico,"JN"); break;
            case 5: strcpy(TagMnemonico,"JNZ"); break;
            case 6: strcpy(TagMnemonico,"JNP"); break;
            case 7: strcpy(TagMnemonico,"JNN"); break;
            case 8: strcpy(TagMnemonico,"NOT"); break;
            case 11: strcpy(TagMnemonico,"PUSH"); break;
            case 12: strcpy(TagMnemonico,"POP"); break;
            case 13: strcpy(TagMnemonico,"CALL"); break;
            case 14: strcpy(TagMnemonico,"RET"); break;
            case 15: strcpy(TagMnemonico,"STOP"); break;
            case 16: strcpy(TagMnemonico,"MOV"); break;
            case 17: strcpy(TagMnemonico,"ADD"); break;
            case 18: strcpy(TagMnemonico,"SUB"); break;
            case 19: strcpy(TagMnemonico,"MUL"); break;
            case 20: strcpy(TagMnemonico,"DIV"); break;
            case 21: strcpy(TagMnemonico,"CMP"); break;
            case 22: strcpy(TagMnemonico,"SHL"); break;
            case 23: strcpy(TagMnemonico,"SHR"); break;
            case 24: strcpy(TagMnemonico,"SAR"); break;
            case 25: strcpy(TagMnemonico,"AND"); break;
            case 26: strcpy(TagMnemonico,"OR"); break;
            case 27: strcpy(TagMnemonico,"XOR"); break;
            case 28: strcpy(TagMnemonico,"SWAP"); break;
            case 29: strcpy(TagMnemonico,"LDL"); break;
            case 30: strcpy(TagMnemonico,"LDH"); break;
            case 31: strcpy(TagMnemonico,"RND"); break;
            default:
                printf("\n INSTRUCCION INVALIDA \n");
                instruccion_invalida = true;
                PosicionFisica=TablaSegmentos[0][1]; // Forzar salida
                break;
        }
        if (((Memoria[PosicionFisica] & 0xF0)>>4) % 2){ // Es de 2 Operando
            op2=true;
            PosicionByte=Memoria[PosicionFisica];
            Set_Operando_Dissasembler(Operando2,Memoria,&PosicionFisica,&acumulador,(PosicionByte & 0xC0) >>6);
            Set_Operando_Dissasembler(Operando,Memoria,&PosicionFisica,&acumulador,(PosicionByte & 0x30) >>4);
        }
         else if (((Memoria[PosicionFisica] & 0xF0)>>4) !=0) { // Es de 1 Operandos
             op1=true;
             PosicionByte=Memoria[PosicionFisica];
             Set_Operando_Dissasembler(Operando,Memoria,&PosicionFisica,&acumulador,(PosicionByte & 0xC0) >>6);
        }
        else { // Es de 0 Operandos
            op0=true;
        }
        if (!instruccion_invalida) {
            if (op0) {
                printf("[%04X] %12X | %s\n",PosicionInstruccion,acumulador,TagMnemonico);
            }
            else if (op1){
                printf("[%04X] %12X | %s   %s\n",PosicionInstruccion,acumulador,TagMnemonico,Operando);
            }
            else if (op2) {
                printf("[%04X] %12X | %s   %s, %s\n",PosicionInstruccion,acumulador,TagMnemonico,Operando, Operando2);
            }
        }
        PosicionFisica++;
    }
    printf("////// FINAL DISSASEMBLER //////\n\n");
}

void CargaPila (unsigned char Memoria[N], int Registros[32], short int TablaSegmentos[8][2], int CuentaPalabras) {
    Registros[7]-=12; // Actualiza SP
    int DirFisica= TablaSegmentos[Registros[7]>>16][0] + (Registros[7] & 0x0000FFFF);
    Memoria[DirFisica+11]= (Registros[31] & 0x000000FF);
    Memoria[DirFisica+10]=  (Registros[31] & 0x0000FF00) >>8;
    Memoria[DirFisica+9]= (Registros[31] & 0x00FF0000) >>16;
    Memoria[DirFisica+8]= (Registros[31] & 0xFF000000) >>24;
    Memoria[DirFisica+7]= (CuentaPalabras & 0x000000FF);
    Memoria[DirFisica+6]= (CuentaPalabras & 0x0000FF00) >>8;
    Memoria[DirFisica+5]= (CuentaPalabras & 0x00FF0000) >>16;
    Memoria[DirFisica+4]= (CuentaPalabras & 0xFF000000) >>24;
    Memoria[DirFisica+3]= (-1 & 0x000000FF);
    Memoria[DirFisica+2]= (-1 & 0x0000FF00) >>8;
    Memoria[DirFisica+1]= (-1 & 0x00FF0000) >>16;
    Memoria[DirFisica]= (-1 & 0xFF000000) >>24;

}