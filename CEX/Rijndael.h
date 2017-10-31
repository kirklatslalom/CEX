﻿// The GPL version 3 License (GPLv3)
// 
// Copyright (c) 2017 vtdev.com
// This file is part of the CEX Cryptographic library.
// 
// This program is free software : you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#ifndef CEX_RIJNDAEL_H
#define CEX_RIJNDAEL_H

#include "CexDomain.h"

NAMESPACE_BLOCK

/**
* \internal
*/

//~~~Rijndael S-Box and Lookup Tables~~~//

static const std::array<uint, 30> Rcon =
{
    0x00000000UL, 0x01000000UL, 0x02000000UL, 0x04000000UL, 0x08000000UL, 0x10000000UL, 0x20000000UL, 0x40000000UL,
    0x80000000UL, 0x1B000000UL, 0x36000000UL, 0x6C000000UL, 0xD8000000UL, 0xAB000000UL, 0x4D000000UL, 0x9A000000UL,
    0x2F000000UL, 0x5E000000UL, 0xBC000000UL, 0x63000000UL, 0xC6000000UL, 0x97000000UL, 0x35000000UL, 0x6A000000UL,
    0xD4000000UL, 0xB3000000UL, 0x7D000000UL, 0xFA000000UL, 0xEF000000UL, 0xC5000000UL
};
    
static const std::array<byte, 256> SBox =
{
    0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
    0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
    0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
    0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
    0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
    0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
    0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
    0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
    0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
    0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
    0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
    0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
    0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
    0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
    0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
    0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};
    
static const std::array<byte, 256> ISBox =
{
    0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, 0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB,
    0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87, 0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB,
    0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D, 0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E,
    0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, 0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25,
    0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92,
    0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA, 0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84,
    0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, 0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06,
    0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02, 0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B,
    0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA, 0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73,
    0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, 0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E,
    0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89, 0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B,
    0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20, 0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4,
    0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, 0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F,
    0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D, 0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF,
    0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0, 0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D
};
    
static const std::array<uint, 256> T0 =
{
    0xC66363A5UL, 0xF87C7C84UL, 0xEE777799UL, 0xF67B7B8DUL, 0xFFF2F20DUL, 0xD66B6BBDUL, 0xDE6F6FB1UL, 0x91C5C554UL,
    0x60303050UL, 0x02010103UL, 0xCE6767A9UL, 0x562B2B7DUL, 0xE7FEFE19UL, 0xB5D7D762UL, 0x4DABABE6UL, 0xEC76769AUL,
    0x8FCACA45UL, 0x1F82829DUL, 0x89C9C940UL, 0xFA7D7D87UL, 0xEFFAFA15UL, 0xB25959EBUL, 0x8E4747C9UL, 0xFBF0F00BUL,
    0x41ADADECUL, 0xB3D4D467UL, 0x5FA2A2FDUL, 0x45AFAFEAUL, 0x239C9CBFUL, 0x53A4A4F7UL, 0xE4727296UL, 0x9BC0C05BUL,
    0x75B7B7C2UL, 0xE1FDFD1CUL, 0x3D9393AEUL, 0x4C26266AUL, 0x6C36365AUL, 0x7E3F3F41UL, 0xF5F7F702UL, 0x83CCCC4FUL,
    0x6834345CUL, 0x51A5A5F4UL, 0xD1E5E534UL, 0xF9F1F108UL, 0xE2717193UL, 0xABD8D873UL, 0x62313153UL, 0x2A15153FUL,
    0x0804040CUL, 0x95C7C752UL, 0x46232365UL, 0x9DC3C35EUL, 0x30181828UL, 0x379696A1UL, 0x0A05050FUL, 0x2F9A9AB5UL,
    0x0E070709UL, 0x24121236UL, 0x1B80809BUL, 0xDFE2E23DUL, 0xCDEBEB26UL, 0x4E272769UL, 0x7FB2B2CDUL, 0xEA75759FUL,
    0x1209091BUL, 0x1D83839EUL, 0x582C2C74UL, 0x341A1A2EUL, 0x361B1B2DUL, 0xDC6E6EB2UL, 0xB45A5AEEUL, 0x5BA0A0FBUL,
    0xA45252F6UL, 0x763B3B4DUL, 0xB7D6D661UL, 0x7DB3B3CEUL, 0x5229297BUL, 0xDDE3E33EUL, 0x5E2F2F71UL, 0x13848497UL,
    0xA65353F5UL, 0xB9D1D168UL, 0x00000000UL, 0xC1EDED2CUL, 0x40202060UL, 0xE3FCFC1FUL, 0x79B1B1C8UL, 0xB65B5BEDUL,
    0xD46A6ABEUL, 0x8DCBCB46UL, 0x67BEBED9UL, 0x7239394BUL, 0x944A4ADEUL, 0x984C4CD4UL, 0xB05858E8UL, 0x85CFCF4AUL,
    0xBBD0D06BUL, 0xC5EFEF2AUL, 0x4FAAAAE5UL, 0xEDFBFB16UL, 0x864343C5UL, 0x9A4D4DD7UL, 0x66333355UL, 0x11858594UL,
    0x8A4545CFUL, 0xE9F9F910UL, 0x04020206UL, 0xFE7F7F81UL, 0xA05050F0UL, 0x783C3C44UL, 0x259F9FBAUL, 0x4BA8A8E3UL,
    0xA25151F3UL, 0x5DA3A3FEUL, 0x804040C0UL, 0x058F8F8AUL, 0x3F9292ADUL, 0x219D9DBCUL, 0x70383848UL, 0xF1F5F504UL,
    0x63BCBCDFUL, 0x77B6B6C1UL, 0xAFDADA75UL, 0x42212163UL, 0x20101030UL, 0xE5FFFF1AUL, 0xFDF3F30EUL, 0xBFD2D26DUL,
    0x81CDCD4CUL, 0x180C0C14UL, 0x26131335UL, 0xC3ECEC2FUL, 0xBE5F5FE1UL, 0x359797A2UL, 0x884444CCUL, 0x2E171739UL,
    0x93C4C457UL, 0x55A7A7F2UL, 0xFC7E7E82UL, 0x7A3D3D47UL, 0xC86464ACUL, 0xBA5D5DE7UL, 0x3219192BUL, 0xE6737395UL,
    0xC06060A0UL, 0x19818198UL, 0x9E4F4FD1UL, 0xA3DCDC7FUL, 0x44222266UL, 0x542A2A7EUL, 0x3B9090ABUL, 0x0B888883UL,
    0x8C4646CAUL, 0xC7EEEE29UL, 0x6BB8B8D3UL, 0x2814143CUL, 0xA7DEDE79UL, 0xBC5E5EE2UL, 0x160B0B1DUL, 0xADDBDB76UL,
    0xDBE0E03BUL, 0x64323256UL, 0x743A3A4EUL, 0x140A0A1EUL, 0x924949DBUL, 0x0C06060AUL, 0x4824246CUL, 0xB85C5CE4UL,
    0x9FC2C25DUL, 0xBDD3D36EUL, 0x43ACACEFUL, 0xC46262A6UL, 0x399191A8UL, 0x319595A4UL, 0xD3E4E437UL, 0xF279798BUL,
    0xD5E7E732UL, 0x8BC8C843UL, 0x6E373759UL, 0xDA6D6DB7UL, 0x018D8D8CUL, 0xB1D5D564UL, 0x9C4E4ED2UL, 0x49A9A9E0UL,
    0xD86C6CB4UL, 0xAC5656FAUL, 0xF3F4F407UL, 0xCFEAEA25UL, 0xCA6565AFUL, 0xF47A7A8EUL, 0x47AEAEE9UL, 0x10080818UL,
    0x6FBABAD5UL, 0xF0787888UL, 0x4A25256FUL, 0x5C2E2E72UL, 0x381C1C24UL, 0x57A6A6F1UL, 0x73B4B4C7UL, 0x97C6C651UL,
    0xCBE8E823UL, 0xA1DDDD7CUL, 0xE874749CUL, 0x3E1F1F21UL, 0x964B4BDDUL, 0x61BDBDDCUL, 0x0D8B8B86UL, 0x0F8A8A85UL,
    0xE0707090UL, 0x7C3E3E42UL, 0x71B5B5C4UL, 0xCC6666AAUL, 0x904848D8UL, 0x06030305UL, 0xF7F6F601UL, 0x1C0E0E12UL,
    0xC26161A3UL, 0x6A35355FUL, 0xAE5757F9UL, 0x69B9B9D0UL, 0x17868691UL, 0x99C1C158UL, 0x3A1D1D27UL, 0x279E9EB9UL,
    0xD9E1E138UL, 0xEBF8F813UL, 0x2B9898B3UL, 0x22111133UL, 0xD26969BBUL, 0xA9D9D970UL, 0x078E8E89UL, 0x339494A7UL,
    0x2D9B9BB6UL, 0x3C1E1E22UL, 0x15878792UL, 0xC9E9E920UL, 0x87CECE49UL, 0xAA5555FFUL, 0x50282878UL, 0xA5DFDF7AUL,
    0x038C8C8FUL, 0x59A1A1F8UL, 0x09898980UL, 0x1A0D0D17UL, 0x65BFBFDAUL, 0xD7E6E631UL, 0x844242C6UL, 0xD06868B8UL,
    0x824141C3UL, 0x299999B0UL, 0x5A2D2D77UL, 0x1E0F0F11UL, 0x7BB0B0CBUL, 0xA85454FCUL, 0x6DBBBBD6UL, 0x2C16163AUL
};
    
static const std::array<uint, 256> T1 =
{
    0xA5C66363UL, 0x84F87C7CUL, 0x99EE7777UL, 0x8DF67B7BUL, 0x0DFFF2F2UL, 0xBDD66B6BUL, 0xB1DE6F6FUL, 0x5491C5C5UL,
    0x50603030UL, 0x03020101UL, 0xA9CE6767UL, 0x7D562B2BUL, 0x19E7FEFEUL, 0x62B5D7D7UL, 0xE64DABABUL, 0x9AEC7676UL,
    0x458FCACAUL, 0x9D1F8282UL, 0x4089C9C9UL, 0x87FA7D7DUL, 0x15EFFAFAUL, 0xEBB25959UL, 0xC98E4747UL, 0x0BFBF0F0UL,
    0xEC41ADADUL, 0x67B3D4D4UL, 0xFD5FA2A2UL, 0xEA45AFAFUL, 0xBF239C9CUL, 0xF753A4A4UL, 0x96E47272UL, 0x5B9BC0C0UL,
    0xC275B7B7UL, 0x1CE1FDFDUL, 0xAE3D9393UL, 0x6A4C2626UL, 0x5A6C3636UL, 0x417E3F3FUL, 0x02F5F7F7UL, 0x4F83CCCCUL,
    0x5C683434UL, 0xF451A5A5UL, 0x34D1E5E5UL, 0x08F9F1F1UL, 0x93E27171UL, 0x73ABD8D8UL, 0x53623131UL, 0x3F2A1515UL,
    0x0C080404UL, 0x5295C7C7UL, 0x65462323UL, 0x5E9DC3C3UL, 0x28301818UL, 0xA1379696UL, 0x0F0A0505UL, 0xB52F9A9AUL,
    0x090E0707UL, 0x36241212UL, 0x9B1B8080UL, 0x3DDFE2E2UL, 0x26CDEBEBUL, 0x694E2727UL, 0xCD7FB2B2UL, 0x9FEA7575UL,
    0x1B120909UL, 0x9E1D8383UL, 0x74582C2CUL, 0x2E341A1AUL, 0x2D361B1BUL, 0xB2DC6E6EUL, 0xEEB45A5AUL, 0xFB5BA0A0UL,
    0xF6A45252UL, 0x4D763B3BUL, 0x61B7D6D6UL, 0xCE7DB3B3UL, 0x7B522929UL, 0x3EDDE3E3UL, 0x715E2F2FUL, 0x97138484UL,
    0xF5A65353UL, 0x68B9D1D1UL, 0x00000000UL, 0x2CC1EDEDUL, 0x60402020UL, 0x1FE3FCFCUL, 0xC879B1B1UL, 0xEDB65B5BUL,
    0xBED46A6AUL, 0x468DCBCBUL, 0xD967BEBEUL, 0x4B723939UL, 0xDE944A4AUL, 0xD4984C4CUL, 0xE8B05858UL, 0x4A85CFCFUL,
    0x6BBBD0D0UL, 0x2AC5EFEFUL, 0xE54FAAAAUL, 0x16EDFBFBUL, 0xC5864343UL, 0xD79A4D4DUL, 0x55663333UL, 0x94118585UL,
    0xCF8A4545UL, 0x10E9F9F9UL, 0x06040202UL, 0x81FE7F7FUL, 0xF0A05050UL, 0x44783C3CUL, 0xBA259F9FUL, 0xE34BA8A8UL,
    0xF3A25151UL, 0xFE5DA3A3UL, 0xC0804040UL, 0x8A058F8FUL, 0xAD3F9292UL, 0xBC219D9DUL, 0x48703838UL, 0x04F1F5F5UL,
    0xDF63BCBCUL, 0xC177B6B6UL, 0x75AFDADAUL, 0x63422121UL, 0x30201010UL, 0x1AE5FFFFUL, 0x0EFDF3F3UL, 0x6DBFD2D2UL,
    0x4C81CDCDUL, 0x14180C0CUL, 0x35261313UL, 0x2FC3ECECUL, 0xE1BE5F5FUL, 0xA2359797UL, 0xCC884444UL, 0x392E1717UL,
    0x5793C4C4UL, 0xF255A7A7UL, 0x82FC7E7EUL, 0x477A3D3DUL, 0xACC86464UL, 0xE7BA5D5DUL, 0x2B321919UL, 0x95E67373UL,
    0xA0C06060UL, 0x98198181UL, 0xD19E4F4FUL, 0x7FA3DCDCUL, 0x66442222UL, 0x7E542A2AUL, 0xAB3B9090UL, 0x830B8888UL,
    0xCA8C4646UL, 0x29C7EEEEUL, 0xD36BB8B8UL, 0x3C281414UL, 0x79A7DEDEUL, 0xE2BC5E5EUL, 0x1D160B0BUL, 0x76ADDBDBUL,
    0x3BDBE0E0UL, 0x56643232UL, 0x4E743A3AUL, 0x1E140A0AUL, 0xDB924949UL, 0x0A0C0606UL, 0x6C482424UL, 0xE4B85C5CUL,
    0x5D9FC2C2UL, 0x6EBDD3D3UL, 0xEF43ACACUL, 0xA6C46262UL, 0xA8399191UL, 0xA4319595UL, 0x37D3E4E4UL, 0x8BF27979UL,
    0x32D5E7E7UL, 0x438BC8C8UL, 0x596E3737UL, 0xB7DA6D6DUL, 0x8C018D8DUL, 0x64B1D5D5UL, 0xD29C4E4EUL, 0xE049A9A9UL,
    0xB4D86C6CUL, 0xFAAC5656UL, 0x07F3F4F4UL, 0x25CFEAEAUL, 0xAFCA6565UL, 0x8EF47A7AUL, 0xE947AEAEUL, 0x18100808UL,
    0xD56FBABAUL, 0x88F07878UL, 0x6F4A2525UL, 0x725C2E2EUL, 0x24381C1CUL, 0xF157A6A6UL, 0xC773B4B4UL, 0x5197C6C6UL,
    0x23CBE8E8UL, 0x7CA1DDDDUL, 0x9CE87474UL, 0x213E1F1FUL, 0xDD964B4BUL, 0xDC61BDBDUL, 0x860D8B8BUL, 0x850F8A8AUL,
    0x90E07070UL, 0x427C3E3EUL, 0xC471B5B5UL, 0xAACC6666UL, 0xD8904848UL, 0x05060303UL, 0x01F7F6F6UL, 0x121C0E0EUL,
    0xA3C26161UL, 0x5F6A3535UL, 0xF9AE5757UL, 0xD069B9B9UL, 0x91178686UL, 0x5899C1C1UL, 0x273A1D1DUL, 0xB9279E9EUL,
    0x38D9E1E1UL, 0x13EBF8F8UL, 0xB32B9898UL, 0x33221111UL, 0xBBD26969UL, 0x70A9D9D9UL, 0x89078E8EUL, 0xA7339494UL,
    0xB62D9B9BUL, 0x223C1E1EUL, 0x92158787UL, 0x20C9E9E9UL, 0x4987CECEUL, 0xFFAA5555UL, 0x78502828UL, 0x7AA5DFDFUL,
    0x8F038C8CUL, 0xF859A1A1UL, 0x80098989UL, 0x171A0D0DUL, 0xDA65BFBFUL, 0x31D7E6E6UL, 0xC6844242UL, 0xB8D06868UL,
    0xC3824141UL, 0xB0299999UL, 0x775A2D2DUL, 0x111E0F0FUL, 0xCB7BB0B0UL, 0xFCA85454UL, 0xD66DBBBBUL, 0x3A2C1616UL
};
    
static const std::array<uint, 256> T2 =
{
    0x63A5C663UL, 0x7C84F87CUL, 0x7799EE77UL, 0x7B8DF67BUL, 0xF20DFFF2UL, 0x6BBDD66BUL, 0x6FB1DE6FUL, 0xC55491C5UL,
    0x30506030UL, 0x01030201UL, 0x67A9CE67UL, 0x2B7D562BUL, 0xFE19E7FEUL, 0xD762B5D7UL, 0xABE64DABUL, 0x769AEC76UL,
    0xCA458FCAUL, 0x829D1F82UL, 0xC94089C9UL, 0x7D87FA7DUL, 0xFA15EFFAUL, 0x59EBB259UL, 0x47C98E47UL, 0xF00BFBF0UL,
    0xADEC41ADUL, 0xD467B3D4UL, 0xA2FD5FA2UL, 0xAFEA45AFUL, 0x9CBF239CUL, 0xA4F753A4UL, 0x7296E472UL, 0xC05B9BC0UL,
    0xB7C275B7UL, 0xFD1CE1FDUL, 0x93AE3D93UL, 0x266A4C26UL, 0x365A6C36UL, 0x3F417E3FUL, 0xF702F5F7UL, 0xCC4F83CCUL,
    0x345C6834UL, 0xA5F451A5UL, 0xE534D1E5UL, 0xF108F9F1UL, 0x7193E271UL, 0xD873ABD8UL, 0x31536231UL, 0x153F2A15UL,
    0x040C0804UL, 0xC75295C7UL, 0x23654623UL, 0xC35E9DC3UL, 0x18283018UL, 0x96A13796UL, 0x050F0A05UL, 0x9AB52F9AUL,
    0x07090E07UL, 0x12362412UL, 0x809B1B80UL, 0xE23DDFE2UL, 0xEB26CDEBUL, 0x27694E27UL, 0xB2CD7FB2UL, 0x759FEA75UL,
    0x091B1209UL, 0x839E1D83UL, 0x2C74582CUL, 0x1A2E341AUL, 0x1B2D361BUL, 0x6EB2DC6EUL, 0x5AEEB45AUL, 0xA0FB5BA0UL,
    0x52F6A452UL, 0x3B4D763BUL, 0xD661B7D6UL, 0xB3CE7DB3UL, 0x297B5229UL, 0xE33EDDE3UL, 0x2F715E2FUL, 0x84971384UL,
    0x53F5A653UL, 0xD168B9D1UL, 0x00000000UL, 0xED2CC1EDUL, 0x20604020UL, 0xFC1FE3FCUL, 0xB1C879B1UL, 0x5BEDB65BUL,
    0x6ABED46AUL, 0xCB468DCBUL, 0xBED967BEUL, 0x394B7239UL, 0x4ADE944AUL, 0x4CD4984CUL, 0x58E8B058UL, 0xCF4A85CFUL,
    0xD06BBBD0UL, 0xEF2AC5EFUL, 0xAAE54FAAUL, 0xFB16EDFBUL, 0x43C58643UL, 0x4DD79A4DUL, 0x33556633UL, 0x85941185UL,
    0x45CF8A45UL, 0xF910E9F9UL, 0x02060402UL, 0x7F81FE7FUL, 0x50F0A050UL, 0x3C44783CUL, 0x9FBA259FUL, 0xA8E34BA8UL,
    0x51F3A251UL, 0xA3FE5DA3UL, 0x40C08040UL, 0x8F8A058FUL, 0x92AD3F92UL, 0x9DBC219DUL, 0x38487038UL, 0xF504F1F5UL,
    0xBCDF63BCUL, 0xB6C177B6UL, 0xDA75AFDAUL, 0x21634221UL, 0x10302010UL, 0xFF1AE5FFUL, 0xF30EFDF3UL, 0xD26DBFD2UL,
    0xCD4C81CDUL, 0x0C14180CUL, 0x13352613UL, 0xEC2FC3ECUL, 0x5FE1BE5FUL, 0x97A23597UL, 0x44CC8844UL, 0x17392E17UL,
    0xC45793C4UL, 0xA7F255A7UL, 0x7E82FC7EUL, 0x3D477A3DUL, 0x64ACC864UL, 0x5DE7BA5DUL, 0x192B3219UL, 0x7395E673UL,
    0x60A0C060UL, 0x81981981UL, 0x4FD19E4FUL, 0xDC7FA3DCUL, 0x22664422UL, 0x2A7E542AUL, 0x90AB3B90UL, 0x88830B88UL,
    0x46CA8C46UL, 0xEE29C7EEUL, 0xB8D36BB8UL, 0x143C2814UL, 0xDE79A7DEUL, 0x5EE2BC5EUL, 0x0B1D160BUL, 0xDB76ADDBUL,
    0xE03BDBE0UL, 0x32566432UL, 0x3A4E743AUL, 0x0A1E140AUL, 0x49DB9249UL, 0x060A0C06UL, 0x246C4824UL, 0x5CE4B85CUL,
    0xC25D9FC2UL, 0xD36EBDD3UL, 0xACEF43ACUL, 0x62A6C462UL, 0x91A83991UL, 0x95A43195UL, 0xE437D3E4UL, 0x798BF279UL,
    0xE732D5E7UL, 0xC8438BC8UL, 0x37596E37UL, 0x6DB7DA6DUL, 0x8D8C018DUL, 0xD564B1D5UL, 0x4ED29C4EUL, 0xA9E049A9UL,
    0x6CB4D86CUL, 0x56FAAC56UL, 0xF407F3F4UL, 0xEA25CFEAUL, 0x65AFCA65UL, 0x7A8EF47AUL, 0xAEE947AEUL, 0x08181008UL,
    0xBAD56FBAUL, 0x7888F078UL, 0x256F4A25UL, 0x2E725C2EUL, 0x1C24381CUL, 0xA6F157A6UL, 0xB4C773B4UL, 0xC65197C6UL,
    0xE823CBE8UL, 0xDD7CA1DDUL, 0x749CE874UL, 0x1F213E1FUL, 0x4BDD964BUL, 0xBDDC61BDUL, 0x8B860D8BUL, 0x8A850F8AUL,
    0x7090E070UL, 0x3E427C3EUL, 0xB5C471B5UL, 0x66AACC66UL, 0x48D89048UL, 0x03050603UL, 0xF601F7F6UL, 0x0E121C0EUL,
    0x61A3C261UL, 0x355F6A35UL, 0x57F9AE57UL, 0xB9D069B9UL, 0x86911786UL, 0xC15899C1UL, 0x1D273A1DUL, 0x9EB9279EUL,
    0xE138D9E1UL, 0xF813EBF8UL, 0x98B32B98UL, 0x11332211UL, 0x69BBD269UL, 0xD970A9D9UL, 0x8E89078EUL, 0x94A73394UL,
    0x9BB62D9BUL, 0x1E223C1EUL, 0x87921587UL, 0xE920C9E9UL, 0xCE4987CEUL, 0x55FFAA55UL, 0x28785028UL, 0xDF7AA5DFUL,
    0x8C8F038CUL, 0xA1F859A1UL, 0x89800989UL, 0x0D171A0DUL, 0xBFDA65BFUL, 0xE631D7E6UL, 0x42C68442UL, 0x68B8D068UL,
    0x41C38241UL, 0x99B02999UL, 0x2D775A2DUL, 0x0F111E0FUL, 0xB0CB7BB0UL, 0x54FCA854UL, 0xBBD66DBBUL, 0x163A2C16UL
};
    
static const std::array<uint, 256> T3 =
{
    0x6363A5C6UL, 0x7C7C84F8UL, 0x777799EEUL, 0x7B7B8DF6UL, 0xF2F20DFFUL, 0x6B6BBDD6UL, 0x6F6FB1DEUL, 0xC5C55491UL,
    0x30305060UL, 0x01010302UL, 0x6767A9CEUL, 0x2B2B7D56UL, 0xFEFE19E7UL, 0xD7D762B5UL, 0xABABE64DUL, 0x76769AECUL,
    0xCACA458FUL, 0x82829D1FUL, 0xC9C94089UL, 0x7D7D87FAUL, 0xFAFA15EFUL, 0x5959EBB2UL, 0x4747C98EUL, 0xF0F00BFBUL,
    0xADADEC41UL, 0xD4D467B3UL, 0xA2A2FD5FUL, 0xAFAFEA45UL, 0x9C9CBF23UL, 0xA4A4F753UL, 0x727296E4UL, 0xC0C05B9BUL,
    0xB7B7C275UL, 0xFDFD1CE1UL, 0x9393AE3DUL, 0x26266A4CUL, 0x36365A6CUL, 0x3F3F417EUL, 0xF7F702F5UL, 0xCCCC4F83UL,
    0x34345C68UL, 0xA5A5F451UL, 0xE5E534D1UL, 0xF1F108F9UL, 0x717193E2UL, 0xD8D873ABUL, 0x31315362UL, 0x15153F2AUL,
    0x04040C08UL, 0xC7C75295UL, 0x23236546UL, 0xC3C35E9DUL, 0x18182830UL, 0x9696A137UL, 0x05050F0AUL, 0x9A9AB52FUL,
    0x0707090EUL, 0x12123624UL, 0x80809B1BUL, 0xE2E23DDFUL, 0xEBEB26CDUL, 0x2727694EUL, 0xB2B2CD7FUL, 0x75759FEAUL,
    0x09091B12UL, 0x83839E1DUL, 0x2C2C7458UL, 0x1A1A2E34UL, 0x1B1B2D36UL, 0x6E6EB2DCUL, 0x5A5AEEB4UL, 0xA0A0FB5BUL,
    0x5252F6A4UL, 0x3B3B4D76UL, 0xD6D661B7UL, 0xB3B3CE7DUL, 0x29297B52UL, 0xE3E33EDDUL, 0x2F2F715EUL, 0x84849713UL,
    0x5353F5A6UL, 0xD1D168B9UL, 0x00000000UL, 0xEDED2CC1UL, 0x20206040UL, 0xFCFC1FE3UL, 0xB1B1C879UL, 0x5B5BEDB6UL,
    0x6A6ABED4UL, 0xCBCB468DUL, 0xBEBED967UL, 0x39394B72UL, 0x4A4ADE94UL, 0x4C4CD498UL, 0x5858E8B0UL, 0xCFCF4A85UL,
    0xD0D06BBBUL, 0xEFEF2AC5UL, 0xAAAAE54FUL, 0xFBFB16EDUL, 0x4343C586UL, 0x4D4DD79AUL, 0x33335566UL, 0x85859411UL,
    0x4545CF8AUL, 0xF9F910E9UL, 0x02020604UL, 0x7F7F81FEUL, 0x5050F0A0UL, 0x3C3C4478UL, 0x9F9FBA25UL, 0xA8A8E34BUL,
    0x5151F3A2UL, 0xA3A3FE5DUL, 0x4040C080UL, 0x8F8F8A05UL, 0x9292AD3FUL, 0x9D9DBC21UL, 0x38384870UL, 0xF5F504F1UL,
    0xBCBCDF63UL, 0xB6B6C177UL, 0xDADA75AFUL, 0x21216342UL, 0x10103020UL, 0xFFFF1AE5UL, 0xF3F30EFDUL, 0xD2D26DBFUL,
    0xCDCD4C81UL, 0x0C0C1418UL, 0x13133526UL, 0xECEC2FC3UL, 0x5F5FE1BEUL, 0x9797A235UL, 0x4444CC88UL, 0x1717392EUL,
    0xC4C45793UL, 0xA7A7F255UL, 0x7E7E82FCUL, 0x3D3D477AUL, 0x6464ACC8UL, 0x5D5DE7BAUL, 0x19192B32UL, 0x737395E6UL,
    0x6060A0C0UL, 0x81819819UL, 0x4F4FD19EUL, 0xDCDC7FA3UL, 0x22226644UL, 0x2A2A7E54UL, 0x9090AB3BUL, 0x8888830BUL,
    0x4646CA8CUL, 0xEEEE29C7UL, 0xB8B8D36BUL, 0x14143C28UL, 0xDEDE79A7UL, 0x5E5EE2BCUL, 0x0B0B1D16UL, 0xDBDB76ADUL,
    0xE0E03BDBUL, 0x32325664UL, 0x3A3A4E74UL, 0x0A0A1E14UL, 0x4949DB92UL, 0x06060A0CUL, 0x24246C48UL, 0x5C5CE4B8UL,
    0xC2C25D9FUL, 0xD3D36EBDUL, 0xACACEF43UL, 0x6262A6C4UL, 0x9191A839UL, 0x9595A431UL, 0xE4E437D3UL, 0x79798BF2UL,
    0xE7E732D5UL, 0xC8C8438BUL, 0x3737596EUL, 0x6D6DB7DAUL, 0x8D8D8C01UL, 0xD5D564B1UL, 0x4E4ED29CUL, 0xA9A9E049UL,
    0x6C6CB4D8UL, 0x5656FAACUL, 0xF4F407F3UL, 0xEAEA25CFUL, 0x6565AFCAUL, 0x7A7A8EF4UL, 0xAEAEE947UL, 0x08081810UL,
    0xBABAD56FUL, 0x787888F0UL, 0x25256F4AUL, 0x2E2E725CUL, 0x1C1C2438UL, 0xA6A6F157UL, 0xB4B4C773UL, 0xC6C65197UL,
    0xE8E823CBUL, 0xDDDD7CA1UL, 0x74749CE8UL, 0x1F1F213EUL, 0x4B4BDD96UL, 0xBDBDDC61UL, 0x8B8B860DUL, 0x8A8A850FUL,
    0x707090E0UL, 0x3E3E427CUL, 0xB5B5C471UL, 0x6666AACCUL, 0x4848D890UL, 0x03030506UL, 0xF6F601F7UL, 0x0E0E121CUL,
    0x6161A3C2UL, 0x35355F6AUL, 0x5757F9AEUL, 0xB9B9D069UL, 0x86869117UL, 0xC1C15899UL, 0x1D1D273AUL, 0x9E9EB927UL,
    0xE1E138D9UL, 0xF8F813EBUL, 0x9898B32BUL, 0x11113322UL, 0x6969BBD2UL, 0xD9D970A9UL, 0x8E8E8907UL, 0x9494A733UL,
    0x9B9BB62DUL, 0x1E1E223CUL, 0x87879215UL, 0xE9E920C9UL, 0xCECE4987UL, 0x5555FFAAUL, 0x28287850UL, 0xDFDF7AA5UL,
    0x8C8C8F03UL, 0xA1A1F859UL, 0x89898009UL, 0x0D0D171AUL, 0xBFBFDA65UL, 0xE6E631D7UL, 0x4242C684UL, 0x6868B8D0UL,
    0x4141C382UL, 0x9999B029UL, 0x2D2D775AUL, 0x0F0F111EUL, 0xB0B0CB7BUL, 0x5454FCA8UL, 0xBBBBD66DUL, 0x16163A2CUL
};
    
static const std::array<uint, 256> IT0 =
{
    0x51F4A750UL, 0x7E416553UL, 0x1A17A4C3UL, 0x3A275E96UL, 0x3BAB6BCBUL, 0x1F9D45F1UL, 0xACFA58ABUL, 0x4BE30393UL,
    0x2030FA55UL, 0xAD766DF6UL, 0x88CC7691UL, 0xF5024C25UL, 0x4FE5D7FCUL, 0xC52ACBD7UL, 0x26354480UL, 0xB562A38FUL,
    0xDEB15A49UL, 0x25BA1B67UL, 0x45EA0E98UL, 0x5DFEC0E1UL, 0xC32F7502UL, 0x814CF012UL, 0x8D4697A3UL, 0x6BD3F9C6UL,
    0x038F5FE7UL, 0x15929C95UL, 0xBF6D7AEBUL, 0x955259DAUL, 0xD4BE832DUL, 0x587421D3UL, 0x49E06929UL, 0x8EC9C844UL,
    0x75C2896AUL, 0xF48E7978UL, 0x99583E6BUL, 0x27B971DDUL, 0xBEE14FB6UL, 0xF088AD17UL, 0xC920AC66UL, 0x7DCE3AB4UL,
    0x63DF4A18UL, 0xE51A3182UL, 0x97513360UL, 0x62537F45UL, 0xB16477E0UL, 0xBB6BAE84UL, 0xFE81A01CUL, 0xF9082B94UL,
    0x70486858UL, 0x8F45FD19UL, 0x94DE6C87UL, 0x527BF8B7UL, 0xAB73D323UL, 0x724B02E2UL, 0xE31F8F57UL, 0x6655AB2AUL,
    0xB2EB2807UL, 0x2FB5C203UL, 0x86C57B9AUL, 0xD33708A5UL, 0x302887F2UL, 0x23BFA5B2UL, 0x02036ABAUL, 0xED16825CUL,
    0x8ACF1C2BUL, 0xA779B492UL, 0xF307F2F0UL, 0x4E69E2A1UL, 0x65DAF4CDUL, 0x0605BED5UL, 0xD134621FUL, 0xC4A6FE8AUL,
    0x342E539DUL, 0xA2F355A0UL, 0x058AE132UL, 0xA4F6EB75UL, 0x0B83EC39UL, 0x4060EFAAUL, 0x5E719F06UL, 0xBD6E1051UL,
    0x3E218AF9UL, 0x96DD063DUL, 0xDD3E05AEUL, 0x4DE6BD46UL, 0x91548DB5UL, 0x71C45D05UL, 0x0406D46FUL, 0x605015FFUL,
    0x1998FB24UL, 0xD6BDE997UL, 0x894043CCUL, 0x67D99E77UL, 0xB0E842BDUL, 0x07898B88UL, 0xE7195B38UL, 0x79C8EEDBUL,
    0xA17C0A47UL, 0x7C420FE9UL, 0xF8841EC9UL, 0x00000000UL, 0x09808683UL, 0x322BED48UL, 0x1E1170ACUL, 0x6C5A724EUL,
    0xFD0EFFFBUL, 0x0F853856UL, 0x3DAED51EUL, 0x362D3927UL, 0x0A0FD964UL, 0x685CA621UL, 0x9B5B54D1UL, 0x24362E3AUL,
    0x0C0A67B1UL, 0x9357E70FUL, 0xB4EE96D2UL, 0x1B9B919EUL, 0x80C0C54FUL, 0x61DC20A2UL, 0x5A774B69UL, 0x1C121A16UL,
    0xE293BA0AUL, 0xC0A02AE5UL, 0x3C22E043UL, 0x121B171DUL, 0x0E090D0BUL, 0xF28BC7ADUL, 0x2DB6A8B9UL, 0x141EA9C8UL,
    0x57F11985UL, 0xAF75074CUL, 0xEE99DDBBUL, 0xA37F60FDUL, 0xF701269FUL, 0x5C72F5BCUL, 0x44663BC5UL, 0x5BFB7E34UL,
    0x8B432976UL, 0xCB23C6DCUL, 0xB6EDFC68UL, 0xB8E4F163UL, 0xD731DCCAUL, 0x42638510UL, 0x13972240UL, 0x84C61120UL,
    0x854A247DUL, 0xD2BB3DF8UL, 0xAEF93211UL, 0xC729A16DUL, 0x1D9E2F4BUL, 0xDCB230F3UL, 0x0D8652ECUL, 0x77C1E3D0UL,
    0x2BB3166CUL, 0xA970B999UL, 0x119448FAUL, 0x47E96422UL, 0xA8FC8CC4UL, 0xA0F03F1AUL, 0x567D2CD8UL, 0x223390EFUL,
    0x87494EC7UL, 0xD938D1C1UL, 0x8CCAA2FEUL, 0x98D40B36UL, 0xA6F581CFUL, 0xA57ADE28UL, 0xDAB78E26UL, 0x3FADBFA4UL,
    0x2C3A9DE4UL, 0x5078920DUL, 0x6A5FCC9BUL, 0x547E4662UL, 0xF68D13C2UL, 0x90D8B8E8UL, 0x2E39F75EUL, 0x82C3AFF5UL,
    0x9F5D80BEUL, 0x69D0937CUL, 0x6FD52DA9UL, 0xCF2512B3UL, 0xC8AC993BUL, 0x10187DA7UL, 0xE89C636EUL, 0xDB3BBB7BUL,
    0xCD267809UL, 0x6E5918F4UL, 0xEC9AB701UL, 0x834F9AA8UL, 0xE6956E65UL, 0xAAFFE67EUL, 0x21BCCF08UL, 0xEF15E8E6UL,
    0xBAE79BD9UL, 0x4A6F36CEUL, 0xEA9F09D4UL, 0x29B07CD6UL, 0x31A4B2AFUL, 0x2A3F2331UL, 0xC6A59430UL, 0x35A266C0UL,
    0x744EBC37UL, 0xFC82CAA6UL, 0xE090D0B0UL, 0x33A7D815UL, 0xF104984AUL, 0x41ECDAF7UL, 0x7FCD500EUL, 0x1791F62FUL,
    0x764DD68DUL, 0x43EFB04DUL, 0xCCAA4D54UL, 0xE49604DFUL, 0x9ED1B5E3UL, 0x4C6A881BUL, 0xC12C1FB8UL, 0x4665517FUL,
    0x9D5EEA04UL, 0x018C355DUL, 0xFA877473UL, 0xFB0B412EUL, 0xB3671D5AUL, 0x92DBD252UL, 0xE9105633UL, 0x6DD64713UL,
    0x9AD7618CUL, 0x37A10C7AUL, 0x59F8148EUL, 0xEB133C89UL, 0xCEA927EEUL, 0xB761C935UL, 0xE11CE5EDUL, 0x7A47B13CUL,
    0x9CD2DF59UL, 0x55F2733FUL, 0x1814CE79UL, 0x73C737BFUL, 0x53F7CDEAUL, 0x5FFDAA5BUL, 0xDF3D6F14UL, 0x7844DB86UL,
    0xCAAFF381UL, 0xB968C43EUL, 0x3824342CUL, 0xC2A3405FUL, 0x161DC372UL, 0xBCE2250CUL, 0x283C498BUL, 0xFF0D9541UL,
    0x39A80171UL, 0x080CB3DEUL, 0xD8B4E49CUL, 0x6456C190UL, 0x7BCB8461UL, 0xD532B670UL, 0x486C5C74UL, 0xD0B85742UL
};
    
static const std::array<uint, 256> IT1 =
{
    0x5051F4A7UL, 0x537E4165UL, 0xC31A17A4UL, 0x963A275EUL, 0xCB3BAB6BUL, 0xF11F9D45UL, 0xABACFA58UL, 0x934BE303UL,
    0x552030FAUL, 0xF6AD766DUL, 0x9188CC76UL, 0x25F5024CUL, 0xFC4FE5D7UL, 0xD7C52ACBUL, 0x80263544UL, 0x8FB562A3UL,
    0x49DEB15AUL, 0x6725BA1BUL, 0x9845EA0EUL, 0xE15DFEC0UL, 0x02C32F75UL, 0x12814CF0UL, 0xA38D4697UL, 0xC66BD3F9UL,
    0xE7038F5FUL, 0x9515929CUL, 0xEBBF6D7AUL, 0xDA955259UL, 0x2DD4BE83UL, 0xD3587421UL, 0x2949E069UL, 0x448EC9C8UL,
    0x6A75C289UL, 0x78F48E79UL, 0x6B99583EUL, 0xDD27B971UL, 0xB6BEE14FUL, 0x17F088ADUL, 0x66C920ACUL, 0xB47DCE3AUL,
    0x1863DF4AUL, 0x82E51A31UL, 0x60975133UL, 0x4562537FUL, 0xE0B16477UL, 0x84BB6BAEUL, 0x1CFE81A0UL, 0x94F9082BUL,
    0x58704868UL, 0x198F45FDUL, 0x8794DE6CUL, 0xB7527BF8UL, 0x23AB73D3UL, 0xE2724B02UL, 0x57E31F8FUL, 0x2A6655ABUL,
    0x07B2EB28UL, 0x032FB5C2UL, 0x9A86C57BUL, 0xA5D33708UL, 0xF2302887UL, 0xB223BFA5UL, 0xBA02036AUL, 0x5CED1682UL,
    0x2B8ACF1CUL, 0x92A779B4UL, 0xF0F307F2UL, 0xA14E69E2UL, 0xCD65DAF4UL, 0xD50605BEUL, 0x1FD13462UL, 0x8AC4A6FEUL,
    0x9D342E53UL, 0xA0A2F355UL, 0x32058AE1UL, 0x75A4F6EBUL, 0x390B83ECUL, 0xAA4060EFUL, 0x065E719FUL, 0x51BD6E10UL,
    0xF93E218AUL, 0x3D96DD06UL, 0xAEDD3E05UL, 0x464DE6BDUL, 0xB591548DUL, 0x0571C45DUL, 0x6F0406D4UL, 0xFF605015UL,
    0x241998FBUL, 0x97D6BDE9UL, 0xCC894043UL, 0x7767D99EUL, 0xBDB0E842UL, 0x8807898BUL, 0x38E7195BUL, 0xDB79C8EEUL,
    0x47A17C0AUL, 0xE97C420FUL, 0xC9F8841EUL, 0x00000000UL, 0x83098086UL, 0x48322BEDUL, 0xAC1E1170UL, 0x4E6C5A72UL,
    0xFBFD0EFFUL, 0x560F8538UL, 0x1E3DAED5UL, 0x27362D39UL, 0x640A0FD9UL, 0x21685CA6UL, 0xD19B5B54UL, 0x3A24362EUL,
    0xB10C0A67UL, 0x0F9357E7UL, 0xD2B4EE96UL, 0x9E1B9B91UL, 0x4F80C0C5UL, 0xA261DC20UL, 0x695A774BUL, 0x161C121AUL,
    0x0AE293BAUL, 0xE5C0A02AUL, 0x433C22E0UL, 0x1D121B17UL, 0x0B0E090DUL, 0xADF28BC7UL, 0xB92DB6A8UL, 0xC8141EA9UL,
    0x8557F119UL, 0x4CAF7507UL, 0xBBEE99DDUL, 0xFDA37F60UL, 0x9FF70126UL, 0xBC5C72F5UL, 0xC544663BUL, 0x345BFB7EUL,
    0x768B4329UL, 0xDCCB23C6UL, 0x68B6EDFCUL, 0x63B8E4F1UL, 0xCAD731DCUL, 0x10426385UL, 0x40139722UL, 0x2084C611UL,
    0x7D854A24UL, 0xF8D2BB3DUL, 0x11AEF932UL, 0x6DC729A1UL, 0x4B1D9E2FUL, 0xF3DCB230UL, 0xEC0D8652UL, 0xD077C1E3UL,
    0x6C2BB316UL, 0x99A970B9UL, 0xFA119448UL, 0x2247E964UL, 0xC4A8FC8CUL, 0x1AA0F03FUL, 0xD8567D2CUL, 0xEF223390UL,
    0xC787494EUL, 0xC1D938D1UL, 0xFE8CCAA2UL, 0x3698D40BUL, 0xCFA6F581UL, 0x28A57ADEUL, 0x26DAB78EUL, 0xA43FADBFUL,
    0xE42C3A9DUL, 0x0D507892UL, 0x9B6A5FCCUL, 0x62547E46UL, 0xC2F68D13UL, 0xE890D8B8UL, 0x5E2E39F7UL, 0xF582C3AFUL,
    0xBE9F5D80UL, 0x7C69D093UL, 0xA96FD52DUL, 0xB3CF2512UL, 0x3BC8AC99UL, 0xA710187DUL, 0x6EE89C63UL, 0x7BDB3BBBUL,
    0x09CD2678UL, 0xF46E5918UL, 0x01EC9AB7UL, 0xA8834F9AUL, 0x65E6956EUL, 0x7EAAFFE6UL, 0x0821BCCFUL, 0xE6EF15E8UL,
    0xD9BAE79BUL, 0xCE4A6F36UL, 0xD4EA9F09UL, 0xD629B07CUL, 0xAF31A4B2UL, 0x312A3F23UL, 0x30C6A594UL, 0xC035A266UL,
    0x37744EBCUL, 0xA6FC82CAUL, 0xB0E090D0UL, 0x1533A7D8UL, 0x4AF10498UL, 0xF741ECDAUL, 0x0E7FCD50UL, 0x2F1791F6UL,
    0x8D764DD6UL, 0x4D43EFB0UL, 0x54CCAA4DUL, 0xDFE49604UL, 0xE39ED1B5UL, 0x1B4C6A88UL, 0xB8C12C1FUL, 0x7F466551UL,
    0x049D5EEAUL, 0x5D018C35UL, 0x73FA8774UL, 0x2EFB0B41UL, 0x5AB3671DUL, 0x5292DBD2UL, 0x33E91056UL, 0x136DD647UL,
    0x8C9AD761UL, 0x7A37A10CUL, 0x8E59F814UL, 0x89EB133CUL, 0xEECEA927UL, 0x35B761C9UL, 0xEDE11CE5UL, 0x3C7A47B1UL,
    0x599CD2DFUL, 0x3F55F273UL, 0x791814CEUL, 0xBF73C737UL, 0xEA53F7CDUL, 0x5B5FFDAAUL, 0x14DF3D6FUL, 0x867844DBUL,
    0x81CAAFF3UL, 0x3EB968C4UL, 0x2C382434UL, 0x5FC2A340UL, 0x72161DC3UL, 0x0CBCE225UL, 0x8B283C49UL, 0x41FF0D95UL,
    0x7139A801UL, 0xDE080CB3UL, 0x9CD8B4E4UL, 0x906456C1UL, 0x617BCB84UL, 0x70D532B6UL, 0x74486C5CUL, 0x42D0B857UL
};
    
static const std::array<uint, 256> IT2 =
{
    0xA75051F4UL, 0x65537E41UL, 0xA4C31A17UL, 0x5E963A27UL, 0x6BCB3BABUL, 0x45F11F9DUL, 0x58ABACFAUL, 0x03934BE3UL,
    0xFA552030UL, 0x6DF6AD76UL, 0x769188CCUL, 0x4C25F502UL, 0xD7FC4FE5UL, 0xCBD7C52AUL, 0x44802635UL, 0xA38FB562UL,
    0x5A49DEB1UL, 0x1B6725BAUL, 0x0E9845EAUL, 0xC0E15DFEUL, 0x7502C32FUL, 0xF012814CUL, 0x97A38D46UL, 0xF9C66BD3UL,
    0x5FE7038FUL, 0x9C951592UL, 0x7AEBBF6DUL, 0x59DA9552UL, 0x832DD4BEUL, 0x21D35874UL, 0x692949E0UL, 0xC8448EC9UL,
    0x896A75C2UL, 0x7978F48EUL, 0x3E6B9958UL, 0x71DD27B9UL, 0x4FB6BEE1UL, 0xAD17F088UL, 0xAC66C920UL, 0x3AB47DCEUL,
    0x4A1863DFUL, 0x3182E51AUL, 0x33609751UL, 0x7F456253UL, 0x77E0B164UL, 0xAE84BB6BUL, 0xA01CFE81UL, 0x2B94F908UL,
    0x68587048UL, 0xFD198F45UL, 0x6C8794DEUL, 0xF8B7527BUL, 0xD323AB73UL, 0x02E2724BUL, 0x8F57E31FUL, 0xAB2A6655UL,
    0x2807B2EBUL, 0xC2032FB5UL, 0x7B9A86C5UL, 0x08A5D337UL, 0x87F23028UL, 0xA5B223BFUL, 0x6ABA0203UL, 0x825CED16UL,
    0x1C2B8ACFUL, 0xB492A779UL, 0xF2F0F307UL, 0xE2A14E69UL, 0xF4CD65DAUL, 0xBED50605UL, 0x621FD134UL, 0xFE8AC4A6UL,
    0x539D342EUL, 0x55A0A2F3UL, 0xE132058AUL, 0xEB75A4F6UL, 0xEC390B83UL, 0xEFAA4060UL, 0x9F065E71UL, 0x1051BD6EUL,
    0x8AF93E21UL, 0x063D96DDUL, 0x05AEDD3EUL, 0xBD464DE6UL, 0x8DB59154UL, 0x5D0571C4UL, 0xD46F0406UL, 0x15FF6050UL,
    0xFB241998UL, 0xE997D6BDUL, 0x43CC8940UL, 0x9E7767D9UL, 0x42BDB0E8UL, 0x8B880789UL, 0x5B38E719UL, 0xEEDB79C8UL,
    0x0A47A17CUL, 0x0FE97C42UL, 0x1EC9F884UL, 0x00000000UL, 0x86830980UL, 0xED48322BUL, 0x70AC1E11UL, 0x724E6C5AUL,
    0xFFFBFD0EUL, 0x38560F85UL, 0xD51E3DAEUL, 0x3927362DUL, 0xD9640A0FUL, 0xA621685CUL, 0x54D19B5BUL, 0x2E3A2436UL,
    0x67B10C0AUL, 0xE70F9357UL, 0x96D2B4EEUL, 0x919E1B9BUL, 0xC54F80C0UL, 0x20A261DCUL, 0x4B695A77UL, 0x1A161C12UL,
    0xBA0AE293UL, 0x2AE5C0A0UL, 0xE0433C22UL, 0x171D121BUL, 0x0D0B0E09UL, 0xC7ADF28BUL, 0xA8B92DB6UL, 0xA9C8141EUL,
    0x198557F1UL, 0x074CAF75UL, 0xDDBBEE99UL, 0x60FDA37FUL, 0x269FF701UL, 0xF5BC5C72UL, 0x3BC54466UL, 0x7E345BFBUL,
    0x29768B43UL, 0xC6DCCB23UL, 0xFC68B6EDUL, 0xF163B8E4UL, 0xDCCAD731UL, 0x85104263UL, 0x22401397UL, 0x112084C6UL,
    0x247D854AUL, 0x3DF8D2BBUL, 0x3211AEF9UL, 0xA16DC729UL, 0x2F4B1D9EUL, 0x30F3DCB2UL, 0x52EC0D86UL, 0xE3D077C1UL,
    0x166C2BB3UL, 0xB999A970UL, 0x48FA1194UL, 0x642247E9UL, 0x8CC4A8FCUL, 0x3F1AA0F0UL, 0x2CD8567DUL, 0x90EF2233UL,
    0x4EC78749UL, 0xD1C1D938UL, 0xA2FE8CCAUL, 0x0B3698D4UL, 0x81CFA6F5UL, 0xDE28A57AUL, 0x8E26DAB7UL, 0xBFA43FADUL,
    0x9DE42C3AUL, 0x920D5078UL, 0xCC9B6A5FUL, 0x4662547EUL, 0x13C2F68DUL, 0xB8E890D8UL, 0xF75E2E39UL, 0xAFF582C3UL,
    0x80BE9F5DUL, 0x937C69D0UL, 0x2DA96FD5UL, 0x12B3CF25UL, 0x993BC8ACUL, 0x7DA71018UL, 0x636EE89CUL, 0xBB7BDB3BUL,
    0x7809CD26UL, 0x18F46E59UL, 0xB701EC9AUL, 0x9AA8834FUL, 0x6E65E695UL, 0xE67EAAFFUL, 0xCF0821BCUL, 0xE8E6EF15UL,
    0x9BD9BAE7UL, 0x36CE4A6FUL, 0x09D4EA9FUL, 0x7CD629B0UL, 0xB2AF31A4UL, 0x23312A3FUL, 0x9430C6A5UL, 0x66C035A2UL,
    0xBC37744EUL, 0xCAA6FC82UL, 0xD0B0E090UL, 0xD81533A7UL, 0x984AF104UL, 0xDAF741ECUL, 0x500E7FCDUL, 0xF62F1791UL,
    0xD68D764DUL, 0xB04D43EFUL, 0x4D54CCAAUL, 0x04DFE496UL, 0xB5E39ED1UL, 0x881B4C6AUL, 0x1FB8C12CUL, 0x517F4665UL,
    0xEA049D5EUL, 0x355D018CUL, 0x7473FA87UL, 0x412EFB0BUL, 0x1D5AB367UL, 0xD25292DBUL, 0x5633E910UL, 0x47136DD6UL,
    0x618C9AD7UL, 0x0C7A37A1UL, 0x148E59F8UL, 0x3C89EB13UL, 0x27EECEA9UL, 0xC935B761UL, 0xE5EDE11CUL, 0xB13C7A47UL,
    0xDF599CD2UL, 0x733F55F2UL, 0xCE791814UL, 0x37BF73C7UL, 0xCDEA53F7UL, 0xAA5B5FFDUL, 0x6F14DF3DUL, 0xDB867844UL,
    0xF381CAAFUL, 0xC43EB968UL, 0x342C3824UL, 0x405FC2A3UL, 0xC372161DUL, 0x250CBCE2UL, 0x498B283CUL, 0x9541FF0DUL,
    0x017139A8UL, 0xB3DE080CUL, 0xE49CD8B4UL, 0xC1906456UL, 0x84617BCBUL, 0xB670D532UL, 0x5C74486CUL, 0x5742D0B8UL
};
    
static const std::array<uint, 256> IT3 =
{
	0xF4A75051UL, 0x4165537EUL, 0x17A4C31AUL, 0x275E963AUL, 0xAB6BCB3BUL, 0x9D45F11FUL, 0xFA58ABACUL, 0xE303934BUL,
	0x30FA5520UL, 0x766DF6ADUL, 0xCC769188UL, 0x024C25F5UL, 0xE5D7FC4FUL, 0x2ACBD7C5UL, 0x35448026UL, 0x62A38FB5UL,
	0xB15A49DEUL, 0xBA1B6725UL, 0xEA0E9845UL, 0xFEC0E15DUL, 0x2F7502C3UL, 0x4CF01281UL, 0x4697A38DUL, 0xD3F9C66BUL,
	0x8F5FE703UL, 0x929C9515UL, 0x6D7AEBBFUL, 0x5259DA95UL, 0xBE832DD4UL, 0x7421D358UL, 0xE0692949UL, 0xC9C8448EUL,
	0xC2896A75UL, 0x8E7978F4UL, 0x583E6B99UL, 0xB971DD27UL, 0xE14FB6BEUL, 0x88AD17F0UL, 0x20AC66C9UL, 0xCE3AB47DUL,
	0xDF4A1863UL, 0x1A3182E5UL, 0x51336097UL, 0x537F4562UL, 0x6477E0B1UL, 0x6BAE84BBUL, 0x81A01CFEUL, 0x082B94F9UL,
	0x48685870UL, 0x45FD198FUL, 0xDE6C8794UL, 0x7BF8B752UL, 0x73D323ABUL, 0x4B02E272UL, 0x1F8F57E3UL, 0x55AB2A66UL,
	0xEB2807B2UL, 0xB5C2032FUL, 0xC57B9A86UL, 0x3708A5D3UL, 0x2887F230UL, 0xBFA5B223UL, 0x036ABA02UL, 0x16825CEDUL,
	0xCF1C2B8AUL, 0x79B492A7UL, 0x07F2F0F3UL, 0x69E2A14EUL, 0xDAF4CD65UL, 0x05BED506UL, 0x34621FD1UL, 0xA6FE8AC4UL,
	0x2E539D34UL, 0xF355A0A2UL, 0x8AE13205UL, 0xF6EB75A4UL, 0x83EC390BUL, 0x60EFAA40UL, 0x719F065EUL, 0x6E1051BDUL,
	0x218AF93EUL, 0xDD063D96UL, 0x3E05AEDDUL, 0xE6BD464DUL, 0x548DB591UL, 0xC45D0571UL, 0x06D46F04UL, 0x5015FF60UL,
	0x98FB2419UL, 0xBDE997D6UL, 0x4043CC89UL, 0xD99E7767UL, 0xE842BDB0UL, 0x898B8807UL, 0x195B38E7UL, 0xC8EEDB79UL,
	0x7C0A47A1UL, 0x420FE97CUL, 0x841EC9F8UL, 0x00000000UL, 0x80868309UL, 0x2BED4832UL, 0x1170AC1EUL, 0x5A724E6CUL,
	0x0EFFFBFDUL, 0x8538560FUL, 0xAED51E3DUL, 0x2D392736UL, 0x0FD9640AUL, 0x5CA62168UL, 0x5B54D19BUL, 0x362E3A24UL,
	0x0A67B10CUL, 0x57E70F93UL, 0xEE96D2B4UL, 0x9B919E1BUL, 0xC0C54F80UL, 0xDC20A261UL, 0x774B695AUL, 0x121A161CUL,
	0x93BA0AE2UL, 0xA02AE5C0UL, 0x22E0433CUL, 0x1B171D12UL, 0x090D0B0EUL, 0x8BC7ADF2UL, 0xB6A8B92DUL, 0x1EA9C814UL,
	0xF1198557UL, 0x75074CAFUL, 0x99DDBBEEUL, 0x7F60FDA3UL, 0x01269FF7UL, 0x72F5BC5CUL, 0x663BC544UL, 0xFB7E345BUL,
	0x4329768BUL, 0x23C6DCCBUL, 0xEDFC68B6UL, 0xE4F163B8UL, 0x31DCCAD7UL, 0x63851042UL, 0x97224013UL, 0xC6112084UL,
	0x4A247D85UL, 0xBB3DF8D2UL, 0xF93211AEUL, 0x29A16DC7UL, 0x9E2F4B1DUL, 0xB230F3DCUL, 0x8652EC0DUL, 0xC1E3D077UL,
	0xB3166C2BUL, 0x70B999A9UL, 0x9448FA11UL, 0xE9642247UL, 0xFC8CC4A8UL, 0xF03F1AA0UL, 0x7D2CD856UL, 0x3390EF22UL,
	0x494EC787UL, 0x38D1C1D9UL, 0xCAA2FE8CUL, 0xD40B3698UL, 0xF581CFA6UL, 0x7ADE28A5UL, 0xB78E26DAUL, 0xADBFA43FUL,
	0x3A9DE42CUL, 0x78920D50UL, 0x5FCC9B6AUL, 0x7E466254UL, 0x8D13C2F6UL, 0xD8B8E890UL, 0x39F75E2EUL, 0xC3AFF582UL,
	0x5D80BE9FUL, 0xD0937C69UL, 0xD52DA96FUL, 0x2512B3CFUL, 0xAC993BC8UL, 0x187DA710UL, 0x9C636EE8UL, 0x3BBB7BDBUL,
	0x267809CDUL, 0x5918F46EUL, 0x9AB701ECUL, 0x4F9AA883UL, 0x956E65E6UL, 0xFFE67EAAUL, 0xBCCF0821UL, 0x15E8E6EFUL,
	0xE79BD9BAUL, 0x6F36CE4AUL, 0x9F09D4EAUL, 0xB07CD629UL, 0xA4B2AF31UL, 0x3F23312AUL, 0xA59430C6UL, 0xA266C035UL,
	0x4EBC3774UL, 0x82CAA6FCUL, 0x90D0B0E0UL, 0xA7D81533UL, 0x04984AF1UL, 0xECDAF741UL, 0xCD500E7FUL, 0x91F62F17UL,
	0x4DD68D76UL, 0xEFB04D43UL, 0xAA4D54CCUL, 0x9604DFE4UL, 0xD1B5E39EUL, 0x6A881B4CUL, 0x2C1FB8C1UL, 0x65517F46UL,
	0x5EEA049DUL, 0x8C355D01UL, 0x877473FAUL, 0x0B412EFBUL, 0x671D5AB3UL, 0xDBD25292UL, 0x105633E9UL, 0xD647136DUL,
	0xD7618C9AUL, 0xA10C7A37UL, 0xF8148E59UL, 0x133C89EBUL, 0xA927EECEUL, 0x61C935B7UL, 0x1CE5EDE1UL, 0x47B13C7AUL,
	0xD2DF599CUL, 0xF2733F55UL, 0x14CE7918UL, 0xC737BF73UL, 0xF7CDEA53UL, 0xFDAA5B5FUL, 0x3D6F14DFUL, 0x44DB8678UL,
	0xAFF381CAUL, 0x68C43EB9UL, 0x24342C38UL, 0xA3405FC2UL, 0x1DC37216UL, 0xE2250CBCUL, 0x3C498B28UL, 0x0D9541FFUL,
	0xA8017139UL, 0x0CB3DE08UL, 0xB4E49CD8UL, 0x56C19064UL, 0xCB84617BUL, 0x32B670D5UL, 0x6C5C7448UL, 0xB85742D0UL
};

NAMESPACE_BLOCKEND
#endif

