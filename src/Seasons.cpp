#include "Seasons.h"

void Season::LoadSettingsAndVerify(CSimpleIniA& a_ini, bool a_writeComment)
{
	const auto& [seasonType, suffix] = ID;

	logger::info("{}", seasonType);

	INI::get_value(a_ini, allowedWorldspaces, seasonType.c_str(), "Worldspaces", a_writeComment ? ";Valid worldspaces" : "");

	INI::get_value(a_ini, swapActivators, seasonType.c_str(), "Activators", a_writeComment ? ";Swap objects of these types for seasonal variants" : "");
	INI::get_value(a_ini, swapFurniture, seasonType.c_str(), "Furniture", nullptr);
	INI::get_value(a_ini, swapMovableStatics, seasonType.c_str(), "Movable Statics", nullptr);
	INI::get_value(a_ini, swapStatics, seasonType.c_str(), "Statics", nullptr);
	INI::get_value(a_ini, swapTrees, seasonType.c_str(), "Trees", nullptr);

	INI::get_value(a_ini, swapObjectLOD, seasonType.c_str(), "Object LOD", a_writeComment ? ";Seasonal LOD must be generated using DynDOLOD Alpha 67/SSELODGen Beta 88 or higher.\n;See https://dyndolod.info/Help/Seasons for more info" : "");
	INI::get_value(a_ini, swapTerrainLOD, seasonType.c_str(), "Terrain LOD", nullptr);
	INI::get_value(a_ini, swapTreeLOD, seasonType.c_str(), "Tree LOD", nullptr);

	INI::get_value(a_ini, swapGrass, seasonType.c_str(), "Grass", a_writeComment ? ";Enable seasonal grass types (eg. snow grass in winter)" : "");

	//make sure LOD has been generated! No need to check form swaps
	const auto check_if_lod_exists = [&](bool& a_swaplod, std::string_view a_lodType, std::string_view a_folderName) {
		if (a_swaplod) {
			const auto worldSpaceName = !allowedWorldspaces.empty() ? allowedWorldspaces[0] : "Tamriel";
			const auto folderPath = fmt::format(a_folderName, worldSpaceName);

			bool exists = false;
			for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
				if (entry.is_regular_file() && entry.path().string().contains(suffix)) {
					exists = true;
					break;
				}
			}

			if (!exists) {
				a_swaplod = false;
				logger::error(" {} LOD files not found! Default LOD will be used instead", a_lodType);
			} else {
				logger::info(" {} LOD files found", a_lodType);
			}
		}
	};

	check_if_lod_exists(swapTerrainLOD, "Terrain", R"(Data\Meshes\Terrain\{})");
	check_if_lod_exists(swapObjectLOD, "Object", R"(Data\Meshes\Terrain\{}\Objects)");
	check_if_lod_exists(swapTreeLOD, "Tree", R"(Data\Meshes\Terrain\{}\Trees)");
}

bool Season::CanSwapGrass() const
{
	return swapGrass;
}

bool Season::CanSwapLOD(const LOD_TYPE a_type) const
{
	switch (a_type) {
	case LOD_TYPE::kTerrain:
		return swapTerrainLOD;
	case LOD_TYPE::kObject:
		return swapObjectLOD;
	case LOD_TYPE::kTree:
		return swapTreeLOD;
	default:
		return false;
	}
}

bool Season::IsLandscapeSwapAllowed() const
{
	return is_in_valid_worldspace();
}

bool Season::IsSwapAllowed(RE::FormType a_formType) const
{
	return is_valid_swap_type(a_formType) && is_in_valid_worldspace();
}

const std::pair<std::string, std::string>& Season::GetID() const
{
	return ID;
}

SEASON Season::GetType() const
{
	return season;
}

FormSwapMap& Season::GetFormSwapMap()
{
	return formMap;
}
