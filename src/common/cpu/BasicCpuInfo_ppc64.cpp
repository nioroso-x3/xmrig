/* XMRig
 * Copyright 2010      Jeff Garzik <jgarzik@pobox.com>
 * Copyright 2012-2014 pooler      <pooler@litecoinpool.org>
 * Copyright 2014      Lucas Jones <https://github.com/lucasjones>
 * Copyright 2014-2016 Wolf9466    <https://github.com/OhGodAPet>
 * Copyright 2016      Jay D Dee   <jayddee246@gmail.com>
 * Copyright 2017-2018 XMR-Stak    <https://github.com/fireice-uk>, <https://github.com/psychocrypt>
 * Copyright 2016-2018 XMRig       <https://github.com/xmrig>, <support@xmrig.com>
 * Copyright 2018 Timothy Pearson  <tpearson@raptorengineering.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>
#include <thread>


#include "common/cpu/BasicCpuInfo.h"


xmrig::BasicCpuInfo::BasicCpuInfo() :
    m_aes(false),
    m_brand(),
    m_threads(std::thread::hardware_concurrency())
{
	FILE* fp;
	char buffer[32768];
	size_t bytes_read;
	char* match;
	char cpu_name[128];
	char model_name[128];
	char processor_variant[128];
	unsigned int id_high = 0;
	unsigned int id_low = 0;
	unsigned char stepping_high;
	unsigned char stepping_low;

	// CPU name
	fp = fopen("/proc/cpuinfo", "r");
	bytes_read = fread (buffer, 1, sizeof (buffer), fp);
	fclose (fp);
	if (bytes_read != 0 && bytes_read != sizeof (buffer))
	{
		buffer[bytes_read] = '\0';
		match = strstr(buffer, "cpu");
		if (match != NULL)
		{
			match = strstr(match, ": ");
			if (match != NULL)
			{
				sscanf(match, ": %s", cpu_name);
				memcpy(m_brand, cpu_name, strlen(cpu_name));
			}
		}
	}

	// CPU model
	fp = fopen("/proc/cpuinfo", "r");
	bytes_read = fread(buffer, 1, sizeof (buffer), fp);
	fclose (fp);
	if (bytes_read != 0 && bytes_read != sizeof (buffer))
	{
		buffer[bytes_read] = '\0';
		match = strstr(buffer, "model");
		if (match != NULL)
		{
			match = strstr(match, ": ");
			if (match != NULL)
			{
				sscanf(match, ": %s", model_name);
				memcpy(m_brand + strlen(m_brand), " ", 1);
				memcpy(m_brand + strlen(m_brand), model_name, strlen(model_name));
			}
		}
	}

	// CPU version
	fp = fopen("/proc/cpuinfo", "r");
	bytes_read = fread (buffer, 1, sizeof (buffer), fp);
	fclose (fp);
	if (bytes_read != 0 && bytes_read != sizeof (buffer))
	{
		buffer[bytes_read] = '\0';
		match = strstr(buffer, "revision");
		if (match != NULL)
		{
			match = strstr(match, "(pvr ");
			if (match != NULL)
			{
				char pvr_string[128];
				sscanf(match, "(pvr %x %x", &id_high, &id_low);
				if ((id_high == 0x4b) || (id_high == 0x4c) || (id_high == 0x4d)) {
					snprintf(processor_variant, 128, "%s", "POWER8");
				}
				else if (id_high == 0x4e) {
					snprintf(processor_variant, 128, "%s", "POWER9");
				}
				else {
					snprintf(processor_variant, 128, "Unknown (PVR high byte %04x)", id_high);
				}
				stepping_high = (((id_low & 0xffff) - 0x1000) & 0xff00) >> 8;
				stepping_low = ((id_low & 0xffff) - 0x1000) & 0xff;
				snprintf(pvr_string, 128, "%s stepping DD%d.%d", processor_variant, stepping_high, stepping_low);
				memcpy(m_brand + strlen(m_brand), ", ", 2);
				memcpy(m_brand + strlen(m_brand), pvr_string, strlen(pvr_string));
			}
		}
	}

	m_aes = true;
}


size_t xmrig::BasicCpuInfo::optimalThreadsCount(size_t memSize, int maxCpuUsage) const
{
    return threads();
}
