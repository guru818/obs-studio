/*
Copyright (C) 2018 by pkv <pkv.stream@gmail.com>, andersama <anderson.john.alexander@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* For full GPL v2 compatibility it is required to build libs with
 * our open source sdk instead of steinberg sdk , see our fork:
 * https://github.com/pkviet/portaudio , branch : openasio
 * If you build with original asio sdk, you are free to do so to the
 * extent that you do not distribute your binaries.
 */

#include "obs-vst3.hpp"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-vst3", "en-US")

#define blog(level, msg, ...) blog(level, "obs-vst3: " msg, ##__VA_ARGS__)

static void free_type_data(void *type_data)
{
	type_data = 0;
}

using VST3Host = PluginHost<VST3PluginFormat>;
using VSTHost  = PluginHost<VSTPluginFormat>;

StringArray get_paths(VSTPluginFormat &f)
{
	UNUSED_PARAMETER(f);
	return paths_2x;
}

StringArray get_paths(VST3PluginFormat &f)
{
	UNUSED_PARAMETER(f);
	return paths;
}

FileSearchPath get_search_paths(VSTPluginFormat &f)
{
	UNUSED_PARAMETER(f);
	return search_2x;
}

FileSearchPath get_search_paths(VST3PluginFormat &f)
{
	UNUSED_PARAMETER(f);
	return search;
}

void set_paths(VSTPluginFormat &f, StringArray p)
{
	paths_2x = p;
}

void set_paths(VST3PluginFormat &f, StringArray p)
{
	paths = p;
}

void set_search_paths(VSTPluginFormat &f, FileSearchPath p)
{
	search_2x = p;
}

void set_search_paths(VST3PluginFormat &f, FileSearchPath p)
{
	search = p;
}


bool obs_module_load(void)
{
	MessageManager::getInstance();

	struct obs_source_info vst3_filter = {};
	vst3_filter.id                     = "vst_filter3";
	vst3_filter.type                   = OBS_SOURCE_TYPE_FILTER;
	vst3_filter.output_flags           = OBS_SOURCE_AUDIO;
	vst3_filter.get_name               = VST3Host::Name;
	vst3_filter.create                 = VST3Host::Create;
	vst3_filter.destroy                = VST3Host::Destroy;
	vst3_filter.update                 = VST3Host::Update;
	vst3_filter.filter_audio           = VST3Host::Filter_Audio;
	vst3_filter.get_properties         = VST3Host::Properties;
	vst3_filter.save                   = VST3Host::Save;
	vst3_filter.type_data              = (void *)true;
	vst3_filter.free_type_data         = free_type_data;

	struct obs_source_info vst_filter = {};
	vst_filter.id                     = "vst_filter3";
	vst_filter.type                   = OBS_SOURCE_TYPE_FILTER;
	vst_filter.output_flags           = OBS_SOURCE_AUDIO;
	vst_filter.get_name               = VSTHost::Name;
	vst_filter.create                 = VSTHost::Create;
	vst_filter.destroy                = VSTHost::Destroy;
	vst_filter.update                 = VSTHost::Update;
	vst_filter.filter_audio           = VSTHost::Filter_Audio;
	vst_filter.get_properties         = VSTHost::Properties;
	vst_filter.save                   = VSTHost::Save;
	vst_filter.type_data              = (void *)true;
	vst_filter.free_type_data         = free_type_data;

	int version = (JUCE_MAJOR_VERSION << 16) | (JUCE_MINOR_VERSION << 8) | JUCE_BUILDNUMBER;
	blog(LOG_INFO, "JUCE Version: (%i) %i.%i.%i", version, JUCE_MAJOR_VERSION, JUCE_MINOR_VERSION,
			JUCE_BUILDNUMBER);

	obs_register_source(&vst3_filter);

	if (vst3_filter.free_type_data) {
		auto rescan_vst3 = [](void * = nullptr) {
			if (vst3format.canScanForPlugins())
				paths = vst3format.searchPathsForPlugins(search, true, true);
		};
		obs_frontend_add_tools_menu_item("Rescan VST3", rescan_vst3, nullptr);
		rescan_vst3();
	}

	if (vst_filter.free_type_data) {
		auto rescan_vst2 = [](void * = nullptr) {
			if (vst2format.canScanForPlugins())
				paths_2x = vst2format.searchPathsForPlugins(search_2x, true, true);
		};
		obs_frontend_add_tools_menu_item("Rescan VST", rescan_vst2, nullptr);
		rescan_vst2();
	}

	return vst3_filter.type_data || vst_filter.type_data;
}
