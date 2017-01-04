#include "nt_evnt.h"

/*
Pitch simply casted,
Velocity assumed to be between 0 and 1, and is scaled appropriately.
*/
vvvv_err_t vvvv_nt_evnt_get_midi_note_on(vvvv_nt_evnt_t *nev,
                                         vvvv_midi_pckt_t *pckt)
{
#ifdef OSX
    pckt->timeStamp = nev->ts;
    pckt->length = 3;
    /* Done this way to preserve channel if present */
    pckt->data[0] &= 0xf0;
    pckt->data[0] |= MIDIMSG_NOTE_ON;
    pckt->data[1] = ((unsigned char)nev->pch) % MIDIMSG_DATA_BYTE_MAX;
    pckt->data[2] = (unsigned char)(nev->vel * MIDIMSG_DATA_BYTE_MAX);
    return vvvv_err_NONE;
#else
    return vvvv_err_NOTIMP;
#endif
} 

/*
Pitch simply casted,
Velocity just set as 0 (even though it is technically possible to have non-zero note off velocities).
*/
static vvvv_err_t vvvv_nt_evnt_get_midi_note_off(vvvv_nt_evnt_t *nev,
                                                 vvvv_midi_pckt_t *pckt)
{
#ifdef OSX
    pckt->timeStamp = nev->ts + nev->dur;
    pckt->length = 3;
    /* Done this way to preserve channel if present */
    pckt->data[0] &= 0xf0;
    pckt->data[0] |= MIDIMSG_NOTE_OFF;
    pckt->data[1] = ((unsigned char)nev->pch) % MIDIMSG_DATA_BYTE_MAX;
    pckt->data[2] = 0;
    return vvvv_err_NONE;
#else
    return vvvv_err_NOTIMP;
#endif
} 

static vvvv_err_t vvvv_nt_evnt_get_midi_pckt_lst(vvvv_nt_evnt_t *nev,
                                                 vvvv_midi_pckt_lst_t *lst,
                                                 size_t sz)
{
    if (sz < VVVV_MIDI_PCKT_LST_SZ(2)) {
        return vvvv_err_EINVAL;
    }
#ifdef OSX
    lst->numPackets = 2;
    vvvv_midi_pckt_t *pckt = &lst->packet[0];
    (void) vvvv_nt_evnt_get_midi_note_on(nev,pckt);
    pckt = MIDIPacketNext(pckt);
    (void) vvvv_nt_evnt_get_midi_note_off(nev,pckt);
    return vvvv_err_NONE;
#else
    return vvvv_err_NOTIMP;
#endif
}
