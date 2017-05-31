/**
 * @file	nevent.c
 * @brief	Implementation of the event kernel
 */

#include "compiler.h"
#include "nevent.h"
#include "cpucore.h"
#include "pccore.h"


	_NEVENT		g_nevent;


void nevent_allreset(void) {

	// Reset everything - すべてをリセット
	memset(&g_nevent, 0, sizeof(g_nevent));
}

void nevent_get1stevent(void) {

	// Set the number of clocks of the shortest event - 最短のイベントのクロック数をセット
	if (g_nevent.readyevents) {
		CPU_BASECLOCK = g_nevent.item[g_nevent.level[0]].clock;
	}
	else {
		// Set the number of clocks when there is no event - イベントがない場合のクロック数をセット
		CPU_BASECLOCK = NEVENT_MAXCLOCK;
	}

	// Set to counter - カウンタへセット
	CPU_REMCLOCK = CPU_BASECLOCK;
}

static void nevent_execute(void) {

	UINT		eventnum;
	UINT		i;
	UINT		curid;
	NEVENTITEM	item;

	eventnum = 0;
	for (i=0; i<g_nevent.waitevents; i++) {
		curid = g_nevent.waitevent[i];
		item = &g_nevent.item[curid];

		// Execute callback - コールバックの実行 
		if (item->proc != NULL) {
			item->proc(item);

			// Next time carry over event check - 次回に持ち越しのイベントのチェック
			if (item->flag & NEVENT_WAIT) {
				g_nevent.waitevent[eventnum++] = curid;
			}
		}
		else {
			item->flag &= ~(NEVENT_WAIT);
		}
		item->flag &= ~(NEVENT_SETEVENT);
	}
	g_nevent.waitevents = eventnum;
}

void nevent_progress(void) {

	UINT		eventnum;
	SINT32		nextbase;
	UINT		i;
	UINT		curid;
	NEVENTITEM	item;

	CPU_CLOCK += CPU_BASECLOCK;
	eventnum = 0;
	nextbase = NEVENT_MAXCLOCK;
	for (i=0; i<g_nevent.readyevents; i++) {
		curid = g_nevent.level[i];
		item = &g_nevent.item[curid];
		item->clock -= CPU_BASECLOCK;
		if (item->clock > 0) {
			// Waiting for an event - イベント待ち中
			g_nevent.level[eventnum++] = curid;
			if (nextbase >= item->clock) {
				nextbase = item->clock;
			}
		}
		else {
			// Event happened - イベント発生
			if (!(item->flag & (NEVENT_SETEVENT | NEVENT_WAIT))) {
				g_nevent.waitevent[g_nevent.waitevents++] = curid;
			}
			item->flag |= NEVENT_SETEVENT;
			item->flag &= ~(NEVENT_ENABLE);
//			TRACEOUT(("event = %x", curid));
		}
	}
	g_nevent.readyevents = eventnum;
	CPU_BASECLOCK = nextbase;
	CPU_REMCLOCK += nextbase;
	nevent_execute();
//	TRACEOUT(("nextbase = %d (%d)", nextbase, CPU_REMCLOCK));
}


void nevent_reset(UINT id) {

	UINT	i;

	// Search events currently in progress - 現在進行してるイベントを検索
	for (i=0; i<g_nevent.readyevents; i++) {
		if (g_nevent.level[i] == id) {
			break;
		}
	}
	// Does the event exist? - イベントは存在した？
	if (i < g_nevent.readyevents) {
		// Splice it if it exists - 存在していたら削る
		g_nevent.readyevents--;
		for (; i<g_nevent.readyevents; i++) {
			g_nevent.level[i] = g_nevent.level[i+1];
		}
	}
}

void nevent_waitreset(UINT id) {

	UINT	i;

	// Search pending events - 現在進行してるイベントを検索
	for (i=0; i<g_nevent.waitevents; i++) {
		if (g_nevent.waitevent[i] == id) {
			break;
		}
	}
	// Does the event exist?
	if (i < g_nevent.waitevents) {
		// Splice it if it exists - 存在していたら削る
		g_nevent.waitevents--;
		for (; i<g_nevent.waitevents; i++) {
			g_nevent.waitevent[i] = g_nevent.waitevent[i+1];
		}
	}
}

void nevent_set(UINT id, SINT32 eventclock, NEVENTCB proc, NEVENTPOSITION absolute) {

	SINT32		clk;
	NEVENTITEM	item;
	UINT		eventid;
	UINT		i;

//	TRACEOUT(("event %d - %xclocks", id, eventclock));

	clk = CPU_BASECLOCK - CPU_REMCLOCK;
	item = &g_nevent.item[id];
	item->proc = proc;
	item->flag = 0;
	if (absolute) {
		item->clock = eventclock + clk;
	}
	else {
		item->clock += eventclock;
	}
#if 0
	if (item->clock < clk) {
		item->clock = clk;
	}
#endif
	// Delete event - イベントの削除
	nevent_reset(id);

	// Search insertion position of event - イベントの挿入位置の検索
	for (eventid=0; eventid<g_nevent.readyevents; eventid++) {
		if (item->clock < g_nevent.item[g_nevent.level[eventid]].clock) {
			break;
		}
	}

	// Insert event - イベントの挿入
	for (i=g_nevent.readyevents; i>eventid; i--) {
		g_nevent.level[i] = g_nevent.level[i-1];
	}
	g_nevent.level[eventid] = id;
	g_nevent.readyevents++;

	// If it was the shortest event ... - もし最短イベントだったら...
	if (eventid == 0) {
		clk = CPU_BASECLOCK - item->clock;
		CPU_BASECLOCK -= clk;
		CPU_REMCLOCK -= clk;
//		TRACEOUT(("reset nextbase -%d (%d)", clock, CPU_REMCLOCK));
	}
}

void nevent_setbyms(UINT id, SINT32 ms, NEVENTCB proc, NEVENTPOSITION absolute) {  // Set the event to fire at a given time

	nevent_set(id, (pccore.realclock / 1000) * ms, proc, absolute);
}

BOOL nevent_iswork(UINT id) {  // Test if event is in progress

	UINT	i;

	// Search events currently in progress - 現在進行してるイベントを検索
	for (i=0; i<g_nevent.readyevents; i++) {
		if (g_nevent.level[i] == id) {
			return(TRUE);
		}
	}
	return(FALSE);
}

SINT32 nevent_getremain(UINT id) {  // Get time in clocks until event fires

	UINT	i;

	// Search events currently in progress - 現在進行してるイベントを検索
	for (i=0; i<g_nevent.readyevents; i++) {
		if (g_nevent.level[i] == id) {
			return(g_nevent.item[id].clock - (CPU_BASECLOCK - CPU_REMCLOCK));
		}
	}
	return(-1);
}

void nevent_forceexit(void) {  // Force kill the event

	if (CPU_REMCLOCK > 0) {
		CPU_BASECLOCK -= CPU_REMCLOCK;
		CPU_REMCLOCK = 0;
	}
}
