//
// Created by marti on 9/5/2025.
//

#include "Mnemonicos.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#define N 16384

int Get_Valor_Registro(int Registro, int Registros[32]) {
    return Registros[Registro & 0x000000FF];
}

int Get_Valor_Inmediato(int Registro) {
    short int valor = Registro & 0x0000FFFF;
    return (int)valor; // Signo extendido
}


int Get_Valor_Memoria(unsigned char Memoria[N], int Registro, int Registros[32], short int TablaSegmentos[8][2]) {
    int aux= (Registro & 0x001F0000)>>16;
    int base=TablaSegmentos[(Registros[aux]&0x00FF0000)>>16][0] + (Registros[aux]&0x0000FFFF);

    int DireccionFisica= base + Registro & 0x0000FFFF;
    return (Memoria[DireccionFisica]<<24) | (Memoria[DireccionFisica+1]<<16) | (Memoria[DireccionFisica+2]<< 8) | Memoria[DireccionFisica+3];

}

int Get_Valor(unsigned char Memoria[N], int Registro, int Registros[32],short int TablaSegmentos[8][2]) {
    if (((Registro & 0xFF000000)>>24 )==1)
        return Get_Valor_Registro(Registro, Registros);
    if (((Registro & 0xFF000000)>>24 )==2)
        return Get_Valor_Inmediato(Registro);
    if (((Registro & 0xFF000000)>>24 )==3)
        return Get_Valor_Memoria(Memoria,Registro,Registros,TablaSegmentos);
}


void Set_Valor(unsigned char Memoria[N],int Registro,int Registros[32],int valor,short int TablaSegmentos[8][2]) {
    if (((Registro & 0xFF000000) >> 24) == 1)
        Set_Valor_Registro(valor,Registro,Registros);
    else if (((Registro & 0xFF000000) >>24) == 3)
        Set_Valor_Memoria(Memoria,valor,Registro,Registros,TablaSegmentos);


}

void Set_Valor_Registro(int valor,int Registro, int Registros[32]) {
    Registros[Registro & 0x0000001F] = valor;
}

void Set_Valor_Memoria(unsigned char Memoria[N],int valor,int Registro, int Registros[32],short int TablaSegmentos[8][2]) {
    int aux= (Registro & 0x001F0000)>>16;
    int base=TablaSegmentos[((Registros[aux]&0x00FF0000 )>>16)][0] + (Registros[aux]&0x0000FFFF);
    int DireccionFisica= 0;
    DireccionFisica= base + (Registro & 0x0000FFFF);

    if (DireccionFisica> TablaSegmentos[((Registros[aux]&0x00FF0000)>>16)][1])
        exit(-5);

    Memoria[DireccionFisica]= (valor & 0xFF000000)>>24;
    Memoria[DireccionFisica+1]= (valor & 0x00FF0000)>>16;
    Memoria[DireccionFisica+2]= (valor & 0x0000FF00)>>8;
    Memoria[DireccionFisica+3]= (valor & 0x000000FF);

    Registros[0]= (Registros[aux] & 0x00FF0000) | (Registro & 0x0000FFFF);
    Registros[1]= 0x00040000 | (DireccionFisica & 0x0000FFFF);
    Registros[2]= valor;
}

void Set_CC(int Registros[32], int resultado) {
    Registros[17] = Registros[17] & 0x3FFFFFFF; // Limpia N y Z
    if (resultado == 0) {
        Registros[17]= Registros[17] | 0x40000000; // Z=1

    } else if (resultado < 0) {
        Registros[17]= Registros[17] | 0x80000000; // N=1
    }
    else {
        Registros[17]= Registros[17] | 0x00000000; // N=0 y Z=0
    }
}



void MOV(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

    Set_Valor(Memoria, Registros[5],Registros, Get_Valor(Memoria,Registros[6],Registros,TablaSegmentos), TablaSegmentos);

}

void ADD(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
     int aux = 0;
    aux += Get_Valor(Memoria,Registros[6],Registros,TablaSegmentos) + Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos);
    Set_Valor(Memoria,Registros[5],Registros,aux,TablaSegmentos);
    Set_CC(Registros,aux);
}

void SUB(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

    int aux = Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos) - Get_Valor(Memoria,Registros[6],Registros,TablaSegmentos);
    Set_Valor(Memoria,Registros[5],Registros,aux,TablaSegmentos);
    Set_CC(Registros,aux);
}

void MUL(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

    int aux = Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos) * Get_Valor(Memoria,Registros[6],Registros,TablaSegmentos);
    Set_Valor(Memoria,Registros[5],Registros,aux,TablaSegmentos);
    Set_CC(Registros,aux);
}

void DIV(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {  // resto -> AC
 int aux1 = Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos);
 int aux2 = Get_Valor(Memoria,Registros[6],Registros,TablaSegmentos);
    if (aux2==0) {
        printf("Division por 0\n");
        exit(-4);
    }
    int resultado = aux1 / aux2;
    int resto = aux1 % aux2;
    Set_Valor(Memoria,Registros[5],Registros,resultado,TablaSegmentos);
    Registros[16] = resto; // AC = resto
    Set_CC(Registros,resultado);
}

void CMP(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
    int aux = Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos) - Get_Valor(Memoria,Registros[6],Registros,TablaSegmentos);
    Set_CC(Registros,aux);
}

void AND(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
    int aux = Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos) & Get_Valor(Memoria,Registros[6],Registros,TablaSegmentos);
    Set_Valor(Memoria,Registros[5],Registros,aux,TablaSegmentos);
    Set_CC(Registros,aux);
}

void OR(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
    int aux = Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos) | Get_Valor(Memoria,Registros[6],Registros,TablaSegmentos);
    Set_Valor(Memoria,Registros[5],Registros,aux,TablaSegmentos);
    Set_CC(Registros,aux);
}

void XOR(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
 int aux = Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos) ^ Get_Valor(Memoria,Registros[6],Registros,TablaSegmentos);
    Set_Valor(Memoria,Registros[5],Registros,aux,TablaSegmentos);
    Set_CC(Registros,aux);
}

void SWAP(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
    int aux1 = Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos);
    int aux2 = Get_Valor(Memoria,Registros[6],Registros,TablaSegmentos);
    Set_Valor(Memoria,Registros[6],Registros,aux1,TablaSegmentos);
    Set_Valor(Memoria,Registros[5],Registros,aux2,TablaSegmentos);

}

void SHL(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
    unsigned int aux= Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos);
    unsigned int resultado = aux << Get_Valor(Memoria,Registros[6],Registros,TablaSegmentos);
    Set_Valor(Memoria,Registros[5],Registros,resultado,TablaSegmentos);
    Set_CC(Registros,resultado);
}

void SHR(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
    unsigned int aux= Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos);
    unsigned int resultado = aux >> Get_Valor(Memoria,Registros[6],Registros,TablaSegmentos);
    Set_Valor(Memoria,Registros[5],Registros,resultado,TablaSegmentos);
    Set_CC(Registros,resultado);
}

void SAR(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
    int aux   = Get_Valor(Memoria, Registros[5], Registros, TablaSegmentos);
    int shift = Get_Valor(Memoria, Registros[6], Registros, TablaSegmentos);


    if (shift >= 32) {
        // Si se desplaza todo, queda solo el signo
        aux = (aux < 0) ? -1 : 0;
    } else if (shift > 0) {
        if (aux < 0) {
            aux = (aux >> shift) | (0xFFFFFFFF << (32 - shift));
        } else {
            aux = aux >> shift;
        }
    }

    Set_Valor(Memoria,Registros[5],Registros,aux,TablaSegmentos);
    Set_CC(Registros,aux);
}

void RND(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
 int resultado = 0, aux = Get_Valor(Memoria,Registros[6],Registros,TablaSegmentos);
    if (aux > 0)
        resultado = rand() % aux;

}

void LDH(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

    int aux= Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos);
    int carga= Get_Valor(Memoria,Registros[6],Registros,TablaSegmentos);
    aux = (aux & 0x0000FFFF) | (carga << 16);
    Set_Valor(Memoria,Registros[5],Registros,aux,TablaSegmentos);
}

void LDL(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]){
    int aux= Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos);
    int carga= Get_Valor(Memoria,Registros[6],Registros,TablaSegmentos);
    aux = (aux & 0xFFFF0000) | (carga & 0x0000FFFF);
    Set_Valor(Memoria,Registros[5],Registros,aux,TablaSegmentos);
}

bool JMP(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
    Registros[3] = Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos);
    return true;
}

bool JZ(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
 if (Registros[17] & 0x40000000) {
     // Z=1
     Registros[3] = Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos);
     return true;
 }
    return false;
}

bool JP(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
    if (!(Registros[17] & 0xC0000000)) {
        Registros[3] = Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos);
        return true;
    }
    return false;
}

bool JN(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
    if (Registros[17] & 0x80000000) {
        // N=1
        Registros[3] = Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos);
        return true;
    }
    return false;
}

bool JNZ(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
    if (!(Registros[17] & 0x40000000)) {
        // Z=0
        Registros[3] = Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos);
        return true;
    }
    return false;
}

bool JNP(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
   if (Registros[17] & 0xC0000000) {
       // N!=0 | Z!=0
       Registros[3] = Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos);
       return true;
   }
    return false;
}

bool JNN(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
   if (!(Registros[17] & 0x80000000)) {
       // N=0
       Registros[3] = Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos);
       return true;
   }
    return false;
}

void NOT(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
 int aux = Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos);
    aux = ~aux;
    Set_Valor(Memoria,Registros[5],Registros,aux,TablaSegmentos);
    Set_CC(Registros,aux);
}

void SYS(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
    int direccionfisica = TablaSegmentos[((Registros[13] & 0xFFFF0000) >> 16)][0] + (Registros[13] & 0x0000FFFF);
    int modo = Registros[10] & 0x000000FF; // 1 byte
    int celdas = Registros[12] & 0x0000FFFF;
    int tamano = (Registros[12] & 0xFFFF0000) >> 16;
    int tipo = Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos);
    int i,j,num;
    int mascara = 0x000000FF;
    char binstr[33];
    if (tipo == 1) {  // READ
        if (modo == 0X10) {  //binario
            for (i=0; i<celdas; i++) {
                mascara = 0x000000FF;
                printf("[%04X] ", direccionfisica);
                scanf("%32s", binstr); // Leer la cadena binaria
                num = (int)strtol(binstr, NULL, 2); // Convertir a entero
                for (j=tamano-1; j>=0; j--) {
                    Memoria[direccionfisica+j] = (num >> (8 * (tamano - 1 - j))) & mascara;
                }

            }
        }
        else
            if (modo == 0X80) {   //hecadecimal
                for (i=0; i<celdas; i++) {
                    mascara = 0x000000FF;
                    printf("[%04X] ", direccionfisica);
                    scanf("%02x",&num);
                    for (j=tamano-1; j>=0; j--) {
                        Memoria[direccionfisica+j] = (num >> (8 * (tamano - 1 - j))) & mascara;
                    }
                }
            }
            else
                if (modo == 0X04) {   //octal
                    for (i=0; i<celdas; i++) {
                        mascara = 0x000000FF;
                        printf("[%04X] ", direccionfisica);
                        scanf("%o",&num);
                        for (j=tamano-1; j>=0; j--) {
                            Memoria[direccionfisica+j] = (num >> (8 * (tamano - 1 - j))) & mascara;
                        }
                    }
                }
                else
                    if (modo == 0X02) {   //caracteres
                        for (i=0; i<celdas; i++) {
                            mascara = 0x000000FF;
                            printf("[%04X] ", direccionfisica);
                            scanf("%c",&num);
                            for (j=tamano-1; j>=0; j--) {
                                Memoria[direccionfisica+j] = (num >> (8 * (tamano - 1 - j))) & mascara;
                            }
                        }
                   }
                   else
                       if (modo == 0X01) {   //decimal
                           for (i=0; i<celdas; i++) {
                               mascara = 0x000000FF;
                               printf("[%04X] ", direccionfisica);
                               scanf("%d",&num);
                               for (j=tamano-1; j>=0; j--) {
                                   Memoria[direccionfisica+j] = (num >> (8 * (tamano - 1 - j))) & mascara;
                               }
                               direccionfisica+=tamano;
                           }
                        }
    }
    else if (tipo == 2) {  // WRITE
        for (i=0; i<celdas; i++) {
            printf("[%04X] ", direccionfisica);
            if (modo & 0X10) { //binario
                num = 0;
                for (j=0; j<tamano; j++) {
                    num |= (Memoria[direccionfisica + j] << (8 * (tamano - 1 - j)));
                }
                printf("0b");
                print_int_binary(num);
                putchar(' ');
            }
            if (modo & 0X08) {   //hecadecimal
                num = 0;
                for (j=0;j<tamano;j++) {
                    num |= (Memoria[direccionfisica + j] << (8 * (tamano - 1 - j)));
                }
                printf( "0x%X ",num);
            }
            if (modo & 0X04) {   //octal
                num = 0;
                for (j=0;j<tamano;j++) {
                    num |= (Memoria[direccionfisica + j] << (8 * (tamano - 1 - j)));
                }
                printf("@%o ", num); // Each octal digit is about 3 bits
            }
            if (modo & 0X02) {   //caracteres

                for (j=0;j<tamano;j++) {
                    num = Memoria[direccionfisica + j];
                    if (num >= 32 && num <= 126)
                        printf("%c", num);
                    else
                        printf(".");
                }
            printf(" ");
            }
            if (modo & 0X01) {   //decimal
                num = 0;
                for (j=0;j<tamano;j++) {
                    num |= (Memoria[direccionfisica + j] << (8 * (tamano - 1 - j)));
                }
                printf("%d ", num);
            }
            direccionfisica += tamano;
            printf("\n");
        }
    }
}


void print_int_binary(int num) {
    int started = 0;
    for (int k = 31; k >= 0; k--) {
        if ((num & (1 << k)) || started) {
            putchar((num & (1 << k)) ? '1' : '0');
            started = 1;
        }
    }
    if (!started) putchar('0');
}

void STOP() {
    exit(0);
}
