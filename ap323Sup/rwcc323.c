
#include "apCommon.h"
#include "AP323.h"

#define FlashCoefficientMemoryAddress 0x3FE000 /* starting address of calibration data */
#define FlashCoefficientIDString 0x3FEFF0      /* ID string starting address */

/*///////////////////////////// M25P10 definitions /////////////////////////////////////////*/
#define PageProgramM25P10 0x02 /* Page program command */
#define ReadM25P10 0x03        /* Read command */
#define ReadStatusM25P10 0x05  /* Read Status command */
#define SectorEraseM25P10 0x20 /* Sector Erase command */
#define WrenM25P10 0x06        /* Write enable command */

#define WIP 0x01       /* Write in progress status */
#define FMAX_TRIES 250 /* FLASH write status reads tries */
/*#define DBG_SPI		0	/ * define to output SPI data */
/*///////////////////////////////////////////////////////////////////////////////////////////*/

static int I0_M25P10(struct cblk323 *c_blk, unsigned char *command_buf, unsigned char *response_buf, unsigned int size);
static int ReadStatus_M25P10(struct cblk323 *c_blk);
static int SectorErase_M25P10(struct cblk323 *c_blk);
static int WriteFlashBlock(struct cblk323 *c_blk, uint32_t address, void *pdata, uint32_t length);
static int ReadByte_M25P10(struct cblk323 *c_blk, unsigned long address, unsigned char *p);
static int BlankCheckFlash(struct cblk323 *c_blk);

/*
{+D}
    SYSTEM:	    Library Software - AP323 Board

    MODULE NAME:    ReadStatus_M25P10() - routine to read status from the M25P10 device.

    VERSION:	    A

    CREATION DATE:  12/01/15

    DESIGNED BY:    FJM

    CODED BY:       FJM

    ABSTRACT:       This module will issue a command to read the status of the M25P10 device.

    CALLING
    SEQUENCE:   static int ReadStatus_M25P10(struct cblk323 *c_blk)

    MODULE TYPE:    int

    I/O RESOURCES:

    SYSTEM
    RESOURCES:

    MODULES
    CALLED:

    REVISIONS:

  DATE    BY        PURPOSE
-------  ----   ------------------------------------------------


{-D}
*/

/*
    MODULES FUNCTIONAL DETAILS:
*/

static int ReadStatus_M25P10(struct cblk323 *c_blk)
{
    unsigned char cmd_buf[2];
    unsigned char rsp_buf[2];

    memset(&rsp_buf[0], 0, sizeof(rsp_buf)); /* empty response buffer */
    cmd_buf[1] = 0;
    cmd_buf[0] = ReadStatusM25P10; /* read status command */

    I0_M25P10(c_blk, &cmd_buf[0], &rsp_buf[0], sizeof(rsp_buf)); /* Issue command */

    /* recover the response */
    return ((int)rsp_buf[1]);
}

/*
{+D}
    SYSTEM:	    Library Software - AP323 Board

    MODULE NAME:    SectorErase_M25P10() - routine to erase a sector from the M25P10 device.

    VERSION:	    A

    CREATION DATE:  12/01/15

    DESIGNED BY:    FJM

    CODED BY:       FJM

    ABSTRACT:       This module will issue a command to erase a sector of the M25P10 device.

    CALLING
    SEQUENCE:   static int SectorErase_M25P10(struct cblk323 *c_blk, unsigned long address)

    MODULE TYPE:    int

    I/O RESOURCES:

    SYSTEM
    RESOURCES:

    MODULES
    CALLED:

    REVISIONS:

  DATE    BY        PURPOSE
-------  ----   ------------------------------------------------


{-D}
*/

/*
    MODULES FUNCTIONAL DETAILS:

    This module will issue a command to erase a sector of the M25P10 device.
    Requires a valid Sector address for the Sector Erase (SE) instruction
*/

static int SectorErase_M25P10(struct cblk323 *c_blk)
{
    unsigned char cmd_buf[4];
    unsigned char rsp_buf[4];
    int i;
    int status = 0;
    unsigned long address;

    /* Send the WREN command (write enable) */
    cmd_buf[0] = WrenM25P10;                       /* write enable command */
    I0_M25P10(c_blk, &cmd_buf[0], &rsp_buf[0], 1); /* Issue command */

    address = FlashCoefficientMemoryAddress;
    cmd_buf[0] = SectorEraseM25P10;              /* sector erase command */
    cmd_buf[1] = (unsigned char)(address >> 16); /* form A23-A16 address byte */
    cmd_buf[2] = (unsigned char)(address >> 8);  /* form A15-A8 address byte */
    cmd_buf[3] = (unsigned char)(address);       /* form lower address byte */

    I0_M25P10(c_blk, &cmd_buf[0], &rsp_buf[0], sizeof(cmd_buf)); /* Issue command */

    /* Poll write in progress bit until it reads as a logic low */
    for (i = 0; i < FMAX_TRIES; i++)
    {
        usleep(10000); /* Linux */
        status = ReadStatus_M25P10(c_blk);
        if ((status & WIP) == 0) /* zero upon write complete */
            break;
    }

    if (i >= FMAX_TRIES)
        return ((int)-1); /* write error */

    return ((int)0);
}

/*
{+D}

    SYSTEM:	    Library Software - AP323 Board

    VERSION:        A

    CREATION DATE:  12/01/15

    DESIGNED BY:    FJM

    CODED BY:       FJM

    ABSTRACT:       This module is used to configure the flash by loading
                    data over the bus.

    CALLING
        SEQUENCE:   status = static int BlankCheckFlash( struct cblk323 *c_blk);
                        Where:
                          c_blk (pointer to structure)
                                Pointer to configuration block structure

    MODULE TYPE:    integer

    I/O
         RESOURCES:


    SYSTEM
         RESOURCES:

    REVISIONS:


  DATE      BY     PURPOSE
---------  ----   -------------------------------------------------------

{-D}
*/

/*
        MODULES FUNCTIONAL DETAILS:
*/

static int BlankCheckFlash(struct cblk323 *c_blk)
{

    /*
             Declare local data areas
    */

    unsigned char verify_data, read_data;
    uint32_t j, k, length;

    /*
            Entry point of routine
    */

    length = 4096; /* get length of data array in bytes */

    for (k = 0, j = 0; j < length; j++)
    {
        ReadByte_M25P10(c_blk, FlashCoefficientMemoryAddress + j, &read_data);

        verify_data = 0xFF;
        if (verify_data != read_data)
        {
            k++;
            printf("ErrCnt=%06d Adr=%06X Expected=%02X Read=%02X\n", k, FlashCoefficientMemoryAddress + j, verify_data, read_data);
        }
    }

    return ((int)k); /* return error count */
}

/*
{+D}
    SYSTEM:	    Library Software - AP323 Board

    MODULE NAME:    Write_FLASH() - routine to write/read a FLASH port.

    VERSION:	    A

    CREATION DATE:  12/01/15

    DESIGNED BY:    FJM

    CODED BY:       FJM

    ABSTRACT:       This module will write a command and operand bytes.
                    Will also receive response bytes.

    CALLING
    SEQUENCE:   int Write_FLASH(struct cblk323 *c_blk, unsigned char *c_buf, unsigned char *r_buf, unsigned int size)

    MODULE TYPE:    int

    I/O RESOURCES:

    SYSTEM
    RESOURCES:

    MODULES
    CALLED:

    REVISIONS:

  DATE    BY        PURPOSE
-------  ----   ------------------------------------------------


{-D}
*/

/*
    MODULES FUNCTIONAL DETAILS:

    This module will write a command and operand bytes using 8 bit data transfers.
    Will also receive response bytes.
*/

static int Write_FLASH(struct cblk323 *c_blk, unsigned char *command_buf, unsigned char *response_buf, unsigned int size)
{
#ifdef DBG_SPI
    unsigned char *cmd_buf = command_buf;
    unsigned char *rsp_buf = response_buf;
#endif /* DBG_SPI */

    unsigned int sz;

    /* transfer data over the FLASH port */
    for (sz = 1; sz <= size; sz++)
    {
        output_byte(c_blk->nHandle, (byte *)&c_blk->brd_ptr->FLASHData, (byte)*command_buf++); /* LSByte */
        *response_buf++ = (unsigned char)input_byte(c_blk->nHandle, (byte *)&c_blk->brd_ptr->FLASHData);
    }

#ifdef DBG_SPI
    int i;
    printf("\n");
    for (i = 0; i < size; i++)
        printf("%02X ", cmd_buf[i]);
    printf("\n");
    for (i = 0; i < size; i++)
        printf("%02X ", rsp_buf[i]);
#endif /* DBG_SPI */

    return (0);
}

/*
{+D}
    SYSTEM:	    Library Software - AP323 Board

    MODULE NAME:    I0_M25P10() - routine to do I/O with the M25P10 device.

    VERSION:	    A

    CREATION DATE:  12/01/15

    DESIGNED BY:    FJM

    CODED BY:       FJM

    ABSTRACT:       This module will do I/O with the M25P10 device.

    CALLING
    SEQUENCE:   static int IO_M25P10(struct cblk323 *c_blk, unsigned char *command_buf, unsigned char *response, unsigned int size)

    MODULE TYPE:    int	0=success, negative=error

    I/O RESOURCES:

    SYSTEM
    RESOURCES:

    MODULES
    CALLED:

    REVISIONS:

  DATE    BY        PURPOSE
-------  ----   ------------------------------------------------


{-D}
*/

/*
    MODULES FUNCTIONAL DETAILS:

    This module will do I/O with the M25P10 device.
*/

static int I0_M25P10(struct cblk323 *c_blk, unsigned char *command_buf, unsigned char *response_buf, unsigned int size)
{
    /* drive the chip select active for the M25P10 device */
    output_byte(c_blk->nHandle, (byte *)&c_blk->brd_ptr->FlashChipSelect, 0);

    Write_FLASH(c_blk, command_buf, response_buf, size);

    /* drive the chip select inactive for the M25P10 device */
    output_byte(c_blk->nHandle, (byte *)&c_blk->brd_ptr->FlashChipSelect, 1);
    return (0);
}

/*
{+D}
    SYSTEM:	    Library Software - AP323 Board

    MODULE NAME:    ReadByte_M25P10() - routine to read 8 bit data from the M25P10 device.

    VERSION:	    A

    CREATION DATE:  12/01/15

    DESIGNED BY:    FJM

    CODED BY:       FJM

    ABSTRACT:       This module will issue a command to read the M25P10 device.

    CALLING
    SEQUENCE:   static int ReadByte_M25P10(struct cblk323 *c_blk, unsigned long address, unsigned char *p)

    MODULE TYPE:    int

    I/O RESOURCES:

    SYSTEM
    RESOURCES:

    MODULES
    CALLED:

    REVISIONS:

  DATE    BY        PURPOSE
-------  ----   ------------------------------------------------


{-D}
*/

/*
    MODULES FUNCTIONAL DETAILS:


    This module will issue a command to read the M25P10 device.
*/

static int ReadByte_M25P10(struct cblk323 *c_blk, unsigned long address, unsigned char *p)
{
    unsigned char cmd_buf[5];
    unsigned char rsp_buf[5];
    int status = 0;

    memset(&cmd_buf[0], 0, sizeof(cmd_buf)); /* empty the command buffer */
    memset(&rsp_buf[0], 0, sizeof(rsp_buf)); /* empty response buffer */

    cmd_buf[0] = ReadM25P10;                     /* read command */
    cmd_buf[1] = (unsigned char)(address >> 16); /* form A23-A16 address byte */
    cmd_buf[2] = (unsigned char)(address >> 8);  /* form A15-A8 address byte */
    cmd_buf[3] = (unsigned char)(address);       /* form lower address byte */

    I0_M25P10(c_blk, &cmd_buf[0], &rsp_buf[0], sizeof(rsp_buf)); /* Issue command */

    /* recover the response */
    if (p)               /* if pointer is non-zero update */
        *p = rsp_buf[4]; /* update callers data */
    else
        status = -1; /* error */

    return (status);
}

/*
{+D}
    SYSTEM:	    Library Software - AP323 Board

    VERSION:        A

    CREATION DATE:  12/01/15

    DESIGNED BY:    FJM

    CODED BY:       FJM

    ABSTRACT:       This module will write to the M25P10 device.

    CALLING
    SEQUENCE:   static int WriteFlashBlock(struct cblk323 *c_blk, uint32_t address, (void*)pdata, uint32_t length )

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM
    RESOURCES:

    MODULES

    CALLED:

    REVISIONS:

  DATE    BY        PURPOSE
-------  ----   ------------------------------------------------


{-D}
*/

/*    MODULES FUNCTIONAL DETAILS:
 */

static int WriteFlashBlock(struct cblk323 *c_blk, uint32_t address, void *pdata, uint32_t length)
{
    unsigned char cmd_buf[264];
    unsigned char rsp_buf[264]; /* worst case length */
    int i;
    int status = 0;

    if (length > 256)
        return ((int)-1); /* error */

    /* Send the WREN command (write enable) */
    cmd_buf[0] = WrenM25P10;                       /* write enable command */
    I0_M25P10(c_blk, &cmd_buf[0], &rsp_buf[0], 1); /* Issue command */

    /* Build up the write command */
    cmd_buf[0] = PageProgramM25P10;              /* Page Program command */
    cmd_buf[1] = (unsigned char)(address >> 16); /* form A23-A16 address byte */
    cmd_buf[2] = (unsigned char)(address >> 8);  /* form A15-A8 address byte */
    cmd_buf[3] = (unsigned char)(address);       /* form lower address byte */

    memcpy(&cmd_buf[4], pdata, length); /* copy the byte data into local buffer */

    I0_M25P10(c_blk, &cmd_buf[0], &rsp_buf[0], (length + 4)); /* Issue command */

    /* Poll write in progress bit until it reads as a logic low.*/
    for (i = 0; i < FMAX_TRIES; i++)
    {
        usleep(10000);
        status = ReadStatus_M25P10(c_blk);
        if ((status & WIP) == 0) /* zero upon write complete */
            break;
    }

    if (i >= FMAX_TRIES)
        return ((int)-2); /* write error */

    return ((int)0);
}

/*
{+D}
    SYSTEM:	    Library Software - AP323 Board

    MODULE NAME:    ReadFlashID323() - routine to read ID data from the device.

    VERSION:	    A

    CREATION DATE:  12/01/15

    DESIGNED BY:    FJM

    CODED BY:       FJM

    ABSTRACT:       This module will issue a command to read the ID bytes of the device.

    CALLING
    SEQUENCE:	int ReadFlashID323(struct cblk323 *c_blk, unsigned char *p)
                unsigned char *p pointer to place to put data read
    MODULE TYPE:    int

    I/O RESOURCES:

    SYSTEM
    RESOURCES:

    MODULES
    CALLED:

    REVISIONS:

  DATE    BY        PURPOSE
-------  ----   ------------------------------------------------


{-D}
*/

/*
    MODULES FUNCTIONAL DETAILS:

*/

int ReadFlashID323(struct cblk323 *c_blk, unsigned char *p)
{
    unsigned char cmd_buf[10];
    unsigned char rsp_buf[10];
    unsigned long address;
    int status = 0;

    memset(&cmd_buf[0], 0, sizeof(cmd_buf)); /* empty the command buffer */
    memset(&rsp_buf[0], 0, sizeof(rsp_buf)); /* empty response buffer */

    address = FlashCoefficientIDString;
    cmd_buf[0] = ReadM25P10;                     /* read command */
    cmd_buf[1] = (unsigned char)(address >> 16); /* form A23-A16 address byte */
    cmd_buf[2] = (unsigned char)(address >> 8);  /* form A15-A8 address byte */
    cmd_buf[3] = (unsigned char)(address);       /* form lower address byte */

    I0_M25P10(c_blk, &cmd_buf[0], &rsp_buf[0], sizeof(rsp_buf)); /* Issue command */

    /* recover the response */
    *p++ = rsp_buf[4];
    *p++ = rsp_buf[5];
    *p++ = rsp_buf[6];
    *p++ = rsp_buf[7];
    *p++ = rsp_buf[8];
    *p = rsp_buf[9];

    return (status);
}

/*
{+D}
    SYSTEM:	    Library Software - AP323 Board

    FILENAME:       rcc323.c

    MODULE NAME:    rcc323 - read calibration coefficients.

    VERSION:	    A

    CREATION DATE:  12/01/15

    DESIGNED BY:    FJM

    CODED BY:	    FJM

    ABSTRACT:       This module is used to read calibration
                    coefficients AP323 board.

    CALLING
        SEQUENCE:   ReadCalCoeffs323(ptr);
                    where:
                        ptr (pointer to structure)
                            Pointer to the configuration block structure.

    MODULE TYPE:    void

    I/O RESOURCES:

    SYSTEM
        RESOURCES:

    MODULES
        CALLED:

    REVISIONS:

  DATE    BY        PURPOSE
-------  ----   ------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTIONAL DETAILS:

    This module is used to perform the read the calibration
    coefficients for the board.  A pointer to the Configuration
    Block will be passed to this routine.  The routine will use a pointer
    within the Configuration Block together with offsets to reference the
    registers on the Board and will transfer information from the
    Board to the Configuration Block.
*/

int ReadCalCoeffs323(struct cblk323 *c_blk)
{

    /*
             Declare local data areas
    */

    uint32_t calv, j, calb;
    int status;

    /*
            Entry point of routine
        Storage configuration for calibration values
    */

    /* 4 reference cal values 0=9.88V, 1=4.94V, 2=2.47V, and 3=1.235V */
    for (calv = 0; calv < 4; calv++)
    {
        j = (FlashCoefficientMemoryAddress + (calv * 8)); /* Flash memory addressing */

        for (calb = 0; calb < 8; calb++) /* 8 bytes for each calibration value */
        {
            /* read bytes */
            status = ReadByte_M25P10(c_blk, (j + calb), ((unsigned char *)&c_blk->RefCalValues[calv][calb]));
            if (status)
                return (status);
        }
    }
    return (0);
}

/*
{+D}
    SYSTEM:	    Library Software - AP323 Board

    FILENAME:       rwcc323.c

    MODULE NAME:    WriteCalCoeffs323 - write AP323 calibration coefficients and I.D.

    VERSION:	    A

    CREATION DATE:  12/01/15

    DESIGNED BY:    FJM

    CODED BY:	    FJM

    ABSTRACT:       This module is used to write calibration coefficients and
            I.D. AP323 board.

    CALLING
        SEQUENCE:   WriteCalCoeffs(ptr);
                    where:
                        ptr (pointer to structure)
                            Pointer to the configuration block structure.

    MODULE TYPE:    void

    I/O RESOURCES:

    SYSTEM
        RESOURCES:

    MODULES
        CALLED:

    REVISIONS:

  DATE	     BY	    PURPOSE
  --------  ----    ------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTIONAL DETAILS:

    This module is used to write the flash I.D. and calibration
    coefficients for the AP323 board.  A pointer to the Configuration
    Block will be passed to this routine.  The routine will use a pointer
    within the Configuration Block together with offsets to reference the
    registers on the Board.
*/

int WriteCalCoeffs323(struct cblk323 *c_blk)
{
    unsigned char ogc_buf[256];
    int status;

    status = SectorErase_M25P10(c_blk);
    if (status)          /* errors */
        return (status); /* flash error */

    status = BlankCheckFlash(c_blk);
    if (status)          /* errors */
        return (status); /* flash error */

    memset(&ogc_buf[0], 0xFF, sizeof(ogc_buf)); /* set buffer to 0xFF */

    memcpy(&ogc_buf[0], &c_blk->RefCalValues[0][0], 32); /* copy ASCII cal values to buffer */

    /* write calibration vlaues to Flash - 64 bytes */
    status = WriteFlashBlock(c_blk, FlashCoefficientMemoryAddress, &ogc_buf[0], 64);

    /* write the ID string to flash */
    memset(&ogc_buf[0], 0xFF, sizeof(ogc_buf)); /* set buffer to 0xFF */

    /* Add the model ID string */
    strcpy((char *)&ogc_buf[0], (const char *)FlashIDString);

    /* write model ID string to Flash - 64 bytes */
    status = WriteFlashBlock(c_blk, FlashCoefficientIDString, &ogc_buf[0], 64);

    return (status);
}
