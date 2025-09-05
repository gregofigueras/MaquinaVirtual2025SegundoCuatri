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

int Set_Valor(int Memoria[N],int Registros[32]) {
    if (Reg)

}

void MOV(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void ADD(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void SUB(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void MUL(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void DIV(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void CMP(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void AND(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void OR(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void XOR(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void SWAP(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void SHL(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void SHR(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void SAR(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void RND(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void SYS(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void LDH(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void LDL(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void JMP(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void JZ(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void JP(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void JN(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void JNZ(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void JNP(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void JNN(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void NOT(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void SYS(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]) {

}

void STOP() {

}


