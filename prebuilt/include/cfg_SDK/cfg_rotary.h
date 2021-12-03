#ifndef CFG_ROTARY_H
#define CFG_ROTARY_H

/*---------------------------------------------------------------------------*
 *                            INCLUDE   DECLARATIONS                         *
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 *                               DATE    TYPES                               *
 *---------------------------------------------------------------------------*/
typedef struct {
    uint8_t rotary1Enable;
    uint8_t rotary2Enable;
    uint8_t rotary3Enable;
    uint8_t rotary4Enable;
    uint8_t debounceTh;
} CFG_ROTARY_T;

#endif
