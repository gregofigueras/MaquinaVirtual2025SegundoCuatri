//
// Created by marti on 9/5/2025.
//

#ifndef MAQUINAVIRTUAL2025_MNEMONICOS_H
#define MAQUINAVIRTUAL2025_MNEMONICOS_H
#define N 16384
#endif //MAQUINAVIRTUAL2025_MNEMONICOS_H


void MOV(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void ADD(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void SUB(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void MUL(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void DIV(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void CMP(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void AND(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void OR(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void XOR(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void SWAP(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void SHL(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void SHR(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void SAR(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void RND(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void SYS(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void LDH(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void LDL(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void JMP(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void JZ(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void JP(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void JN(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void JNZ(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void JNP(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void JNN(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void NOT(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void SYS(unsigned char Memoria[N], int Registros[32],short int TablaSegmentos[8][2]);
void print_byte_binary(unsigned char byte);
void STOP();

void Set_Valor(unsigned char Memoria[N],int Registro,int Registros[32],int valor,short int TablaSegmentos[8][2]);
void Set_Valor_Registro(int valor,int Registro, int Registros[32]);
void Set_Valor_Memoria(unsigned char Memoria[N], int valor,int Registro, int Registros[32],short int TablaSegmentos[8][2]);
