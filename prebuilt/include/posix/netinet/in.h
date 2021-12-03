/******************************************************************************/
/**
*
* \file	 	in.h
*
* \brief	This file is part of the GNU C Library \n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*\author       email@sunplus.com
*\version	 v0.01
*\date 	 2016/04/15
******************************************************************************/
#ifndef __IN_H__
#define __IN_H__
#include "types.h"

/******************************************************************************************/
/**
 * \fn		uint32_t htonl(uint32_t n)
 *
 * \brief		converts the unsigned integer hostlong from host byte order to network byte order
 *
 * \param		n:(Input)unsigned integer hostlong
 *
 * \return	       network byte order
 *
 ******************************************************************************************/
uint32_t htonl(uint32_t n);

/******************************************************************************************/
/**
 * \fn		uint16_t htons(uint16_t n)
 *
 * \brief		converts the unsigned short integer hostshort from host byte order to network byte order.
 *
 * \param		n:(Input)unsigned short integer hostlong
 *
 * \return	       network byte order
 *
 ******************************************************************************************/
uint16_t htons(uint16_t n);

/******************************************************************************************/
/**
 * \fn		uint32_t ntohl (uint32_t n)
 *
 * \brief		converts the unsigned integer netlong from network byte order to host byte order.
 *
 * \param		n:(Input)unsigned integer netlong
 *
 * \return	       host byte order
 *
 ******************************************************************************************/
uint32_t ntohl (uint32_t n);

/******************************************************************************************/
/**
 * \fn		uint16_t ntohs(uint16_t n)
 *
 * \brief		converts the unsigned short integer netlong from network byte order to host byte order.
 *
 * \param		n:(Input)unsigned short integer netlong
 *
 * \return	       host byte order
 *
 ******************************************************************************************/
uint16_t ntohs(uint16_t n);

#endif //__IN_H__

