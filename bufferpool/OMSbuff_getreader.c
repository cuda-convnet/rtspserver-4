/* * 
 *  $Id: OMSbuff_getreader.c 333 2006-03-31 18:40:05Z shawill $
 *  
 *  This file is part of Fenice
 *
 *  Fenice -- Open Media Server
 *
 *  Copyright (C) 2004 by
 *  	
 *	- Giampaolo Mancini	<giampaolo.mancini@polito.it>
 *	- Francesco Varano	<francesco.varano@polito.it>
 *	- Marco Penno		<marco.penno@polito.it>
 *	- Federico Ridolfo	<federico.ridolfo@polito.it>
 *	- Eugenio Menegatti 	<m.eu@libero.it>
 *	- Stefano Cau
 *	- Giuliano Emma
 *	- Stefano Oldrini
 * 
 *  Fenice is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Fenice is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Fenice; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *  
 * */

#include <stdio.h>

#include <fenice/bufferpool.h>

/*! \return the next slot in the buffer and do NOT move current read position.
 * NULL if buffer is empty
 * */
OMSSlot *OMSbuff_getreader(OMSConsumer *cons)
{
    OMSSlot *last_read;
    OMSSlot *next;

    OMSbuff_lock(cons->buffer);

    // TODO: if it fails?
    OMSbuff_shm_refresh(cons->buffer);

    last_read = OMStoSlot(cons->buffer, cons->last_read_pos);
    next = &cons->buffer->slots[cons->read_pos];

    if ( !next->refs || (next->slot_seq < cons->last_seq) ) {
    // added some slots?
    if ( last_read && cons->buffer->slots[last_read->next].refs && \
    (cons->buffer->slots[last_read->next].slot_seq > cons->last_seq) )
    next = &cons->buffer->slots[last_read->next];
    else {
    OMSbuff_unlock(cons->buffer);
    return NULL;
    }
    } else if (last_read && ( cons->buffer->slots[last_read->next].slot_seq < next->slot_seq ) )
    next = &cons->buffer->slots[last_read->next];

    if (cons->firstts == -1)
    cons->firstts = next->timestamp;

    OMSbuff_unlock(cons->buffer);
    return next;
}

