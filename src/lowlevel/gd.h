/*
    This file is part of Sylverant PSO Patcher
    Copyright (C) 2011 Lawrence Sebald

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3 as
    published by  the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GD_GD_H
#define GD_GD_H

/* Read a single sector off the disc */
int gd_read_sector(uint32_t fad, uint16_t *out, uint32_t *sz);

/* Read the TOC off the disc */
int gd_gd_read_toc(uint16_t *out, uint32_t *sz);

#endif /* GD_GD_H */
