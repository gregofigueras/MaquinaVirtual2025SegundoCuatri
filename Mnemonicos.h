//
// Created by marti on 9/5/2025.
//

#ifndef MAQUINAVIRTUAL2025_MNEMONICOS_H
#define MAQUINAVIRTUAL2025_MNEMONICOS_H
#define N 16384
#endif //MAQUINAVIRTUAL2025_MNEMONICOS_H


void MOV(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void ADD(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void SUB(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void MUL(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void DIV(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void CMP(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void AND(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void OR(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void XOR(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void SWAP(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void SHL(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void SHR(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void SAR(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void RND(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void SYS(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void LDH(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void LDL(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void JMP(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void JZ(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void JP(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void JN(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void JNZ(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void JNP(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void JNN(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void NOT(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void SYS(int Memoria[N], int Registros[32],short int TablaSegmentos[8][2];
void STOP();

void Set_Valor(int Memoria[N],int Registro,int Registros[32],int valor,short int TablaSegmentos[8][2]);
void Set_Valor_Registro(int valor,int Registro, int Registros[32]);
void Set_Valor_Memoria(int Memoria[N], int valor,int Registro, int Registros[32],short int TablaSegmentos[8][2]);
