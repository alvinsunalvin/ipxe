/*
 * Copyright (C) 2007 Michael Brown <mbrown@fensystems.co.uk>.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <errno.h>
#include <gpxe/process.h>
#include <console.h>
#include <gpxe/keys.h>
#include <gpxe/job.h>
#include <gpxe/monojob.h>

/** @file
 *
 * Single foreground job
 *
 */

static int monojob_rc;

static void monojob_done ( struct job_interface *job __unused, int rc ) {
	monojob_rc = rc;
}

/** Single foreground job operations */
static struct job_interface_operations monojob_operations = {
	.done		= monojob_done,
	.kill		= ignore_job_kill,
	.progress	= ignore_job_progress,
};

/** Single foreground job */
struct job_interface monojob = {
	.intf = {
		.dest = &null_job.intf,
		.refcnt = NULL,
	},
	.op = &monojob_operations,
};

/**
 * Wait for single foreground job to complete
 *
 * @ret rc		Job final status code
 */
int monojob_wait ( void ) {
	int key;

	monojob_rc = -EINPROGRESS;
	while ( monojob_rc == -EINPROGRESS ) {
		step();
		if ( iskey() ) {
			key = getchar();
			switch ( key ) {
			case CTRL_C:
				job_kill ( &monojob );
				return -ECANCELED;
				break;
			default:
				break;
			}
		}
	}
	return monojob_rc;
}
