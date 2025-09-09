//
// Created by marti on 9/5/2025.
//

#include "Mnemonicos.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "Mnemonicos.h"
#include <ctype.h>
#define N 16384

int Get_Valor(int Memoria[N], int Registro, int Registros[32]) {
    if (((Registro & 0xFF000000)>>24 )==1)
        return Get_Valor_Registro(Registro);
    else if (((Registro & 0xFF000000)>>24 )==2)
        return Get_Valor_Inmediato();
    else if (((Registro & 0xFF000000)>>24 )==3)
        return Get_Valor_Memoria();
}

void Set_Valor(int Memoria[N],int Registro,int Registros[32],int valor,short int TablaSegmentos[8][2]) {
    if (((Registro & 0xFF000000) >> 24) == 1)
        Set_Valor_Registro(valor,Registro,Registros);
    else if (((Registro & 0xFF000000) >>24) == 3)
        Set_Valor_Memoria(Memoria,valor,Registro,Registros,TablaSegmentos);


}

void Set_Valor_Registro(int valor,int Registro, int Registros[32]) {
    Registros[Registro & 0x0000001F] = valor;
}

void Set_Valor_Memoria(int Memoria[N],int valor,int Registro, int Registros[32],short int TablaSegmentos[8][2]) {
    int aux= (Registro & 0x001F0000)>>16;
    int base=TablaSegmentos[(Registros[aux]&0x00FF0000)][0] + (Registros[aux]&0x0000FFFF);
    int DireccionFisica= 0;
    DireccionFisica= base + (Registros[5] & 0x0000FFFF);

    Memoria[DireccionFisica]= (valor & 0xFF000000)>>24;
    Memoria[DireccionFisica+1]= (valor & 0x00FF0000)>>16;
    Memoria[DireccionFisica+2]= (valor & 0x0000FF00)>>8;
    Memoria[DireccionFisica+3]= (valor & 0x000000FF);

    Registros[0]=Registros[aux]&0x00FF0000 | Registro & 0x0000FFFF;
    Registros[1]=0x00040000 | DireccionFisica & 0x0000FFFF;
    Registros[2]=valor;
}

void Set_CC(int Registros[32], int resultado) {
    Registros[17] = Registros[17] & 0x3FFFFFFF; // Limpia N y Z
    if (resultado == 0) {
        Registros[17]= Registros[17] | 0x40000000; // Z=1

    } else if (resultado < 0) {
        Registros[17]= Registros[17] | 0x80000000; // N=1
    }
}



void MOV(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

    Set_Valor(Memoria, Registros[5],Registros, Get_Valor(Memoria,Registros[6],Registros,TablaSegmentos), TablaSegmentos);

}

void ADD(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void SUB(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void MUL(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void DIV(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {  // resto -> AC
 int aux1 = Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos);
 int aux2 = Get_Valor(Memoria,Registros[6],Registros,TablaSegmentos);
    if (aux2==0) {
        exit(-4);
    }
 int resultado = aux1 / aux2;
 int resto = aux1 % aux2;
    Set_Valor(Memoria,Registros[5],Registros,resultado,TablaSegmentos);
    Registros[16] = resto; // AC = resto
    Set_CC(Registros,resultado);
}

void CMP(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
    int aux = Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos) - Get_Valor(Memoria,Registros[6],Registros,TablaSegmentos);
    Set_CC(Registros,aux);
}

void AND(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
    int aux = Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos) & Get_Valor(Memoria,Registros[6],Registros,TablaSegmentos);
    Set_Valor(Memoria,Registros[5],Registros,aux,TablaSegmentos);
    Set_CC(Registros,aux);
}

void OR(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
    int aux = Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos) | Get_Valor(Memoria,Registros[6],Registros,TablaSegmentos);
    Set_Valor(Memoria,Registros[5],Registros,aux,TablaSegmentos);
    Set_CC(Registros,aux);
}

void XOR(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
 int aux = Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos) ^ Get_Valor(Memoria,Registros[6],Registros,TablaSegmentos);
    Set_Valor(Memoria,Registros[5],Registros,aux,TablaSegmentos);
    Set_CC(Registros,aux);
}

void SWAP(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
    int aux1 = Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos);
    int aux2 = Get_Valor(Memoria,Registros[6],Registros,TablaSegmentos);
    Set_Valor(Memoria,Registros[6],Registros,aux1,TablaSegmentos);
    Set_Valor(Memoria,Registros[5],Registros,aux2,TablaSegmentos);

}

void SHL(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void SHR(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void SAR(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void RND(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
 int resultado = 0, aux = Get_Valor(Memoria,Registros[6],Registros,TablaSegmentos);
    if (aux > 0)
        resultado = rand() % aux;

}

void SYS(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void LDH(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void LDL(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void JMP(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
    Registros[3] = Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos);
}

void JZ(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
 if (Registros[17] & 0x40000000) // Z=1
     Registros[3] = Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos);
}

void JP(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
    if (!(Registros[17] & 0xC0000000))
        Registros[3] = Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos);
}

void JN(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
    if (Registros[17] & 0x80000000) // N=1
        Registros[3] = Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos);
}

void JNZ(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
    if (!(Registros[17] & 0x40000000)) // Z=0
        Registros[3] = Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos);
}

void JNP(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
   if (Registros[17] & 0xC0000000) // N!=0 | Z!=0
       Registros[3] = Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos);
}

void JNN(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
   if (!(Registros[17] & 0x80000000)) // N=0
     Registros[3] = Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos);
}

void NOT(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
 int aux = Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos);
    aux = ~aux;
    Set_Valor(Memoria,Registros[5],Registros,aux,TablaSegmentos);
    Set_CC(Registros,aux);
}

void SYS(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {
    int posinicial = Memoria[Registros[13]];
    int direccionfisica = TablaSegmentos[((Registros[26] & 0xFFFF0000) >> 16)][0] + posinicial;
    int modo = Memoria[Registros[10]] & 0x000000FF; // 1 byte
    int celdas = Memoria[Registros[12]] & 0x0000FFFF;
    int tamano = (Memoria[Registros[12]] & 0xFFFF0000) >> 16;
    int tipo = Get_Valor(Memoria,Registros[5],Registros,TablaSegmentos);
    int i;

    if (tipo == 1) {  // WRITE
        if (modo == 0X10) {  //binario
            for (i=0; i<celdas; i++) {
                printf("[%0X4] ", direccionfisica);

            }
        }
        else
            if (modo == 0X80) {   //hecadecimal

            }
            else
                if (modo == 0X04) {   //octal

                }
                else
                    if (modo == 0X02) {   //caracteres

                   }
                   else
                       if (modo == 0X01) {   //decimal

                        }
    }
    else if (tipo == 2) {
        if (modo == 0X10) {  //binario

        }
        else
            if (modo == 0X80) {   //hecadecimal

            }
            else
                if (modo == 0X04) {   //octal

                }
                else
                    if (modo == 0X02) {   //caracteres

                    }
                    else
                        if (modo == 0X01) {   //decimal

                        }
    }

}

void STOP() {

}


