#include "../include/kasm.h"
#include "../include/defs.h"
#include "../include/ints.h"
#include "../include/kc.h"
#include "../include/pci.h"
#include "../include/video.h"
#include "../include/shell.h"
#include "../include/programas.h"
#include "../include/stdio.h"
#include "../include/string.h"
#include "../include/paging.h"


DESCR_INT idt[0x81];			/* IDT de 10 entradas*/
IDTR idtr;				/* IDTR */
proceso_t procesos[MAXPROCESOS];
proceso_t init;

int pidActual = 0;
int proxPid = 0;

int Init(int argc, char **argv)
{
  while(1) {
    asm volatile ("hlt");
  }
}

void
LoadIDT(void)
{
        /*Timer Tick*/
        setup_IDT_entry (&idt[0x08], 0x08, (dword)&_int_08_hand, ACS_INT, 0);
        setup_IDT_entry (&idt[0x80], 0x08, (dword)&_int_80_hand, ACS_INT, 0);
        /* Por default el teclado esta en ingles */
        setup_IDT_entry (&idt[0x09], 0x08, (dword)&_int_09_hand_US,ACS_INT, 0);
        /*Division by Zero.*/
        setup_IDT_entry (&idt[0x00], 0x08, (dword)&div0_hand, ACS_INT, 0);
	/*Bounds Error*/
        setup_IDT_entry (&idt[0x05], 0x08, (dword)&bounds_hand, ACS_INT, 0);
	/*Invalid Operation Code*/
        setup_IDT_entry (&idt[0x06], 0x08, (dword)&opCode_hand, ACS_INT, 0);    
	/*Segment Not Present*/
        setup_IDT_entry (&idt[0x0B], 0x08, (dword)&snoPresent_hand, ACS_INT, 0);
	/*Stack Segment Overhead*/
        setup_IDT_entry (&idt[0x0C], 0x08, (dword)&ssf_hand, ACS_INT, 0);   
	/*General Protection Fault*/
        setup_IDT_entry (&idt[0x0D], 0x08, (dword)&generalPfault_hand, ACS_INT, 0); 
	/*Page Fault*/
        setup_IDT_entry (&idt[0x0E], 0x08, (dword)&pageFault_hand, ACS_INT, 0); 

	
        /*Carga de IDTR*/
        idtr.base = 0;
        idtr.base +=(dword) &idt;
        idtr.limit = sizeof(idt)-1;

        _lidt (&idtr);
}

void
kmain()
{
        _Cli();

        IniciarMultiTarea();
        InicializarMemUsuario();
	
	LoadIDT();

        /*Seteo la mascara del PIC para habilitar las interrupciones.*/
        byte aux = 0xFC;
        write(PICM1,&aux,1 );
        aux = 0xFF;
        write(PICM2,&aux,1 );

	/*Inicializo la paginacion*/
	InitPaging();
	
	CrearProceso("INIT", Init, 0, (char **)0, 2, 0, 0x1000);
	
        CrearProceso ("shell", shell, 0, (char **) 0, 2, 0, 0x1000);
	
	_Sti();

        while (1) {
        }
}

void
loadkeysla(void)
{
	setup_IDT_entry (&idt[0x09], 0x08, (dword)&_int_09_hand_LAT,ACS_INT, 0);
}

void
loadkeysus(void)
{
	setup_IDT_entry (&idt[0x09], 0x08, (dword)&_int_09_hand_US,ACS_INT, 0);
}
