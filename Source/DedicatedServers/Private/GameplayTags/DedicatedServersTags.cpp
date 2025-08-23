

#include "GameplayTags/DedicatedServersTags.h"
#include "NativeGameplayTags.h" //already included in .h

namespace DedicatedServersTags
{
	namespace GameSessionsAPI
	{
	//"on GameSession API" kind of weird to me, it is "on some fleet" lol, but anyway.
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ListFleets, "DedicatedServersTags.GameSessionsAPI.ListFleets", "ListFleets resource on GameSession API  - GET");

		UE_DEFINE_GAMEPLAY_TAG_COMMENT(FindOrCreateGameSession, "DedicatedServersTags.GameSessionsAPI.FindOrCreateGameSession", "Find an ACTIVE game session or create one if it doesn't exist on GameSession API - POST");
		
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(CreatePlayerSession, "DedicatedServersTags.GameSessionsAPI.CreatePlayerSession", "CreatePlayerSession on GameSession API - POST");
	}
}
