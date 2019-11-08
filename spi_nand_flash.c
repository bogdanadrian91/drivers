/*-------------------------------------------------------------------------------/
 * spi_nand_flash.c																 /
 *  Created on: Sept 08, 2019													 /
 *    Author: Nevermind															 /
/-------------------------------------------------------------------------------*/
/*
 *  This file contains low level I/O function implementations
 */



#include "spi_nand_flash.h"
#include "fsl_edma.h"

#define EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER kDSPI_MasterPcs0
#define EXAMPLE_DSPI_DEALY_COUNT 0xfffffU



byte frame[4];			 // used for SPI frames ( commands,addresses and whatnots)
byte local_res[]={0};    // used in page read for the operationd success/fail status
DRESULT res;			 // result object of type DRESULT for storing results


DRESULT PageProgram(SPI_Type *EXAMPLE_DSPI_MASTER_BASEADDR,dspi_master_edma_handle_t *g_dspi_edma_m_handle,byte* TXrambuffer,byte* RXrambuffer,uint32_t block,uint32_t page,uint32_t offset,uint32_t length)
{

	/*

	 ########  Page Program Operation  #########
				 1. Write Enable
	 *2. Program Load
	 *3. Program Execute
	 *4. Get Feature
	 */

	/*1.Write Enable*/


	*(frame+0) = Write_Enable;
	dspi_half_duplex_transfer_t masterXfer;

	masterXfer.txData =frame;
	masterXfer.rxData =RXrambuffer;
	masterXfer.txDataSize=1;
	masterXfer.rxDataSize=1;
	masterXfer.configFlags=kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous ;
	masterXfer.isPcsAssertInTransfer=true;
	masterXfer.isTransmitFirst=true;


	if (kStatus_Success != DSPI_MasterHalfDuplexTransferEDMA(EXAMPLE_DSPI_MASTER_BASEADDR, g_dspi_edma_m_handle, &masterXfer) )
	{
		printf("There is error when start DSPI_MasterTransferEDMA in 1.Write Enable  in Page Program Function \r\n ");
	}


	/*Delay to wait slave is ready*/

	for (int v = 0U; v < EXAMPLE_DSPI_DEALY_COUNT; v++)
	{
		__NOP();
	}



	/*2.Program Load*/


	int mask=0;
	mask = ( (Program_Load << 24) | (offset << 8) );
	byte* handle = (byte*)&mask;

	*(frame+0) =  *(handle+3*sizeof(byte));
	*(frame+1)  = *(handle+2*sizeof(byte));
	*(frame+2)  = *(handle+1*sizeof(byte));

	dspi_half_duplex_transfer_t masterT;

	masterT.txData =frame;//
	masterT.rxData =RXrambuffer;
	masterT.txDataSize=3;
	masterT.rxDataSize=0;
	masterT.configFlags=kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous ;
	masterT.isPcsAssertInTransfer=true;
	masterT.isTransmitFirst=true;

	if (kStatus_Success != DSPI_MasterHalfDuplexTransferEDMA(EXAMPLE_DSPI_MASTER_BASEADDR, g_dspi_edma_m_handle, &masterT) )
	{
		printf("There is error when start DSPI_MasterTransferEDMA in 2.Program Load  in Page Read Function \r\n ");
	}

	dspi_half_duplex_transfer_t masterS;

	masterS.txData =TXrambuffer;
	masterS.rxData =RXrambuffer;
	masterS.txDataSize=length;
	masterS.rxDataSize=1;
	masterS.configFlags=kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous ;
	masterS.isPcsAssertInTransfer=true;
	masterS.isTransmitFirst=true;

	if (kStatus_Success != DSPI_MasterHalfDuplexTransferEDMA(EXAMPLE_DSPI_MASTER_BASEADDR, g_dspi_edma_m_handle, &masterS) )
	{
		printf("There is error when start DSPI_MasterTransferEDMA in 2.Program Load in Page Program Function \r\n ");
	}



	/*3. Program Execute*/


	int mk = 0;
	mk = ( (Program_Execute << 24) | (block << 6) ) | page;

	byte* hand =(byte*)&mk;

	*(frame+0) =  *(hand+3*sizeof(byte));
	*(frame+1)  = *(hand+2*sizeof(byte));
	*(frame+2)  = *(hand+1*sizeof(byte));
	*(frame+3)  = *hand;

	dspi_half_duplex_transfer_t masterA;

	masterA.txData =frame;
	masterA.rxData =RXrambuffer;
	masterA.txDataSize=4;
	masterA.rxDataSize=1;
	masterA.configFlags=kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous ;
	masterA.isPcsAssertInTransfer=true;
	masterA.isTransmitFirst=true;

	if (kStatus_Success != DSPI_MasterHalfDuplexTransferEDMA(EXAMPLE_DSPI_MASTER_BASEADDR, g_dspi_edma_m_handle, &masterA) )

	{
		printf("There is error when start DSPI_MasterTransferEDMA in 1.Read Cell Array  in Page Read Function \r\n ");
	}



	for (int p = 0U; p < EXAMPLE_DSPI_DEALY_COUNT; p++)
	{
		__NOP();
	}


	/*4. Program Execute*/



	*(frame+0) = Get_Feature;
	*(frame+1) = StatusRegister;

	dspi_half_duplex_transfer_t masterX;

	masterX.txData =frame;
	masterX.rxData =RXrambuffer;
	masterX.txDataSize=2;
	masterX.rxDataSize=1;
	masterX.configFlags=kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous ;
	masterX.isPcsAssertInTransfer=true;
	masterX.isTransmitFirst=true;


	if (kStatus_Success != DSPI_MasterHalfDuplexTransferEDMA(EXAMPLE_DSPI_MASTER_BASEADDR, g_dspi_edma_m_handle, &masterX) )

	{
		printf("There is error when start DSPI_MasterTransferEDMA in Get Feature in Page Program Function \r\n ");

	}

	if( !(*RXrambuffer & 8) )
		return RES_ERROR;
	else return RES_OK;

}



DRESULT PageRead(SPI_Type *EXAMPLE_DSPI_MASTER_BASEADDR,dspi_master_edma_handle_t *g_dspi_edma_m_handle,byte* RXrambuffer,uint32_t block,uint32_t page,uint32_t offset, uint32_t length )
{
	/*
	 ########  Page Program Operation  #########
	 * 1. Read Cell Array
	 * 2. Get Feature
	 * 3. Read Buffer
	 */


	/* 1. Read Cell Array*/


	int mask = 0;
	mask = ( (Read_Cell_Array << 24) | (block << 6) ) | page;

	byte* handle =(byte*)&mask;

	*(frame+0) =  *(handle+3*sizeof(byte));
	*(frame+1)  = *(handle+2*sizeof(byte));
	*(frame+2)  = *(handle+1*sizeof(byte));
	*(frame+3)  = *handle;

	dspi_half_duplex_transfer_t masterXfer;

	masterXfer.txData =frame;
	masterXfer.rxData =RXrambuffer;
	masterXfer.txDataSize=4;
	masterXfer.rxDataSize=1;
	masterXfer.configFlags=kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous ;
	masterXfer.isPcsAssertInTransfer=true;
	masterXfer.isTransmitFirst=true;


	if (kStatus_Success != DSPI_MasterHalfDuplexTransferEDMA(EXAMPLE_DSPI_MASTER_BASEADDR, g_dspi_edma_m_handle, &masterXfer) )
	{
		printf("There is error when start DSPI_MasterTransferEDMA in 1.Read Cell Array  in Page Read Function \r\n ");
	}


	/*Delay to wait slave is ready*/

	for (int z = 0U; z < EXAMPLE_DSPI_DEALY_COUNT; z++)
	{
		__NOP();
	}


	/*2. Get Feature */


	*(frame+0)  = Get_Feature;
	*(frame+1)  = FeatureTableC0;

	dspi_half_duplex_transfer_t masterX;

	masterX.txData =frame;
	masterX.rxData =local_res;
	masterX.txDataSize=2;
	masterX.rxDataSize=1;
	masterX.configFlags=kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous ;
	masterX.isPcsAssertInTransfer=true;
	masterX.isTransmitFirst=true;


	if (kStatus_Success != DSPI_MasterHalfDuplexTransferEDMA(EXAMPLE_DSPI_MASTER_BASEADDR, g_dspi_edma_m_handle, &masterX) )
	{
		printf("There is error when start DSPI_MasterTransferEDMA in 2.Get Feature in Page Read Function \r\n ");

	}



	int msk =0;
	msk =( (Read_NAND_buffer <<24) | (offset << 8 ) );
	byte* hndl= (byte*)&msk;


	*(frame+0)  = *(hndl+3*sizeof(byte));
	*(frame+1)  = *(hndl+2*sizeof(byte));
	*(frame+2)  = *(hndl+1*sizeof(byte));
	*(frame+3)  = 0;

	dspi_half_duplex_transfer_t masterS;

	masterS.txData =frame;
	masterS.rxData =RXrambuffer;
	masterS.txDataSize=4 ;
	masterS.rxDataSize=length;
	masterS.configFlags=kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous ;
	masterS.isPcsAssertInTransfer=true;
	masterS.isTransmitFirst=true;



	if (kStatus_Success != DSPI_MasterHalfDuplexTransferEDMA(EXAMPLE_DSPI_MASTER_BASEADDR, g_dspi_edma_m_handle, &masterS) )

	{
		printf("There is error when start DSPI_MasterTransferEDMA in 3.Read Buffer in Page Read Function \r\n ");
	}


	if( !( local_res & 1) )
		return RES_OK;
	else return RES_NOTRDY;

}



DRESULT BlockErase(SPI_Type *EXAMPLE_DSPI_MASTER_BASEADDR,dspi_master_edma_handle_t *g_dspi_edma_m_handle ,byte* RXrambuffer,uint32_t block)
{
	/*	 ########  Page Program Operation  #########
	 *  1.Write Enable
	 *  2.Block Erase
	 *  3.Get Feature*/


	/*1. Write Enable*/


	*(frame+0) = Write_Enable;
	dspi_half_duplex_transfer_t masterXfer;

	masterXfer.txData =frame;
	masterXfer.rxData =RXrambuffer;
	masterXfer.txDataSize=1;
	masterXfer.rxDataSize=1;
	masterXfer.configFlags=kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous ;
	masterXfer.isPcsAssertInTransfer=true;
	masterXfer.isTransmitFirst=true;


	if (kStatus_Success != DSPI_MasterHalfDuplexTransferEDMA(EXAMPLE_DSPI_MASTER_BASEADDR, g_dspi_edma_m_handle, &masterXfer) )

	{
		printf("There is error when start DSPI_MasterTransferEDMA in 1.Write Enable  in BlockErase Function \r\n ");
	}


	/*Delay to wait slave is ready*/

	for (int v = 0U; v < EXAMPLE_DSPI_DEALY_COUNT; v++)
	{
		__NOP();
	}



		/*2. Block Erase*/

		int mask = 0;
		mask = ( (Block_Erase << 24) | (block << 6) ) | page;

		byte* handle =(byte*)&mask;

		*(frame+0) =  *(handle+3*sizeof(byte));
		*(frame+1)  = *(handle+2*sizeof(byte));
		*(frame+2)  = *(handle+1*sizeof(byte));
		*(frame+3)  = *handle;

		dspi_half_duplex_transfer_t masterXf;

		masterXf.txData =frame;
		masterXf.rxData =RXrambuffer;
		masterXf.txDataSize=4;
		masterXf.rxDataSize=1;
		masterXf.configFlags=kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous ;
		masterXf.isPcsAssertInTransfer=true;
		masterXf.isTransmitFirst=true;

		if (kStatus_Success != DSPI_MasterHalfDuplexTransferEDMA(EXAMPLE_DSPI_MASTER_BASEADDR, g_dspi_edma_m_handle, &masterXf) )
		{
			printf("There is error when start DSPI_MasterTransferEDMA in Block Erase Function \r\n ");
		}

		/*3. Get Feature*/



		*(frame+0) = Get_Feature;
		*(frame+1) = StatusRegister;


		dspi_half_duplex_transfer_t masterW;

		masterW.txData =frame;
		masterW.rxData =local_res;
		masterW.txDataSize=2;
		masterW.rxDataSize=1;
		masterW.configFlags=kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous ;
		masterW.isPcsAssertInTransfer=true;
		masterW.isTransmitFirst=true;

		if (kStatus_Success != DSPI_MasterHalfDuplexTransferEDMA(EXAMPLE_DSPI_MASTER_BASEADDR, g_dspi_edma_m_handle, &masterW) )
		{
			printf("There is error when start DSPI_MasterTransferEDMA in 3.Get Feature in Block Erase Function \r\n ");

		}


		if( !( local_res & 1) )
			return RES_OK;
		else return RES_NOTRDY;


}



void SetFeature(SPI_Type *EXAMPLE_DSPI_MASTER_BASEADDR,dspi_master_edma_handle_t *g_dspi_edma_m_handle,byte* RXrambuffer,uint32_t reg,uint32_t data)
{


	*(frame+0) = Write_Enable;
	dspi_half_duplex_transfer_t masterXfer;

	masterXfer.txData =frame;
	masterXfer.rxData =RXrambuffer;
	masterXfer.txDataSize=1;
	masterXfer.rxDataSize=1;
	masterXfer.configFlags=kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous ;
	masterXfer.isPcsAssertInTransfer=true;
	masterXfer.isTransmitFirst=true;


	if (kStatus_Success != DSPI_MasterHalfDuplexTransferEDMA(EXAMPLE_DSPI_MASTER_BASEADDR, g_dspi_edma_m_handle, &masterXfer) )
	{
		printf("There is error when start DSPI_MasterTransferEDMA in Set Feature Function \r\n ");
	}


	*(frame+0) = Set_Feature;
	*(frame+1) =(byte)reg;
	*(frame+2) = (byte)data;
	dspi_half_duplex_transfer_t masterW;

	masterW.txData =frame;
	masterW.rxData =RXrambuffer;
	masterW.txDataSize=3;
	masterW.rxDataSize=1;
	masterW.configFlags=kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous ;
	masterW.isPcsAssertInTransfer=true;
	masterW.isTransmitFirst=true;


	if (kStatus_Success != DSPI_MasterHalfDuplexTransferEDMA(EXAMPLE_DSPI_MASTER_BASEADDR, g_dspi_edma_m_handle, &masterW) )

		{
		printf("There is error when start DSPI_MasterTransferEDMA in 2.Get Feature in Page Program Function \r\n ");

		}

}



void Reset_Command(SPI_Type *EXAMPLE_DSPI_MASTER_BASEADDR,dspi_master_edma_handle_t *g_dspi_edma_m_handle){


	*(frame+0) = Reset;
	dspi_half_duplex_transfer_t masterXfer;

	masterXfer.txData =frame;
	masterXfer.rxData =masterRxData;
	masterXfer.txDataSize=1;
	masterXfer.rxDataSize=1;
	masterXfer.configFlags=kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous ;
	masterXfer.isPcsAssertInTransfer=true;
	masterXfer.isTransmitFirst=true;

	if (kStatus_Success != DSPI_MasterHalfDuplexTransferEDMA(EXAMPLE_DSPI_MASTER_BASEADDR, g_dspi_edma_m_handle, &masterXfer) )

	{
		printf("There is error when start DSPI_MasterTransferEDMA in Reset Command Function \r\n ");
	}

}



void ReadID(SPI_Type *EXAMPLE_DSPI_MASTER_BASEADDR,dspi_master_edma_handle_t *g_dspi_edma_m_handle)
{

	frame[0]=0x9F;
	frame[1]=0x0;

	dspi_half_duplex_transfer_t masterXfer;

	masterXfer.txData =frame;
	masterXfer.rxData =masterRxData;
	masterXfer.txDataSize=2;
	masterXfer.rxDataSize=4;
	masterXfer.configFlags=kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous ;
	masterXfer.isPcsAssertInTransfer=true;
	masterXfer.isTransmitFirst=true;



	if (kStatus_Success != DSPI_MasterHalfDuplexTransferEDMA(EXAMPLE_DSPI_MASTER_BASEADDR, g_dspi_edma_m_handle, &masterXfer) )
	{
		printf("There is error when start DSPI_MasterTransferEDMA in ReadID \r\n ");
	}

}



byte GetFeature(SPI_Type *EXAMPLE_DSPI_MASTER_BASEADDR,dspi_master_edma_handle_t *g_dspi_edma_m_handle, byte reg)
{

			frame[0]=0x0F;
			frame[1]=reg;

			dspi_half_duplex_transfer_t masterXfer;

			masterXfer.txData =frame;
	        masterXfer.rxData =local_res;
	        masterXfer.txDataSize=2;
	        masterXfer.rxDataSize=1;
	        masterXfer.configFlags=kDSPI_MasterCtar0 | EXAMPLE_DSPI_MASTER_PCS_FOR_TRANSFER | kDSPI_MasterPcsContinuous ;
	        masterXfer.isPcsAssertInTransfer=true;
	        masterXfer.isTransmitFirst=true;

	  if (kStatus_Success != DSPI_MasterHalfDuplexTransferEDMA(EXAMPLE_DSPI_MASTER_BASEADDR, g_dspi_edma_m_handle, &masterXfer) )

	        {
	            printf("There is error when start DSPI_MasterTransferEDMA in ReadID \r\n ");
	        }

	  return local_res;
}








