

#include "GameplayTags/DedicatedServersTags.h"
#include "NativeGameplayTags.h" //already included in .h

namespace DedicatedServersTags
{
	//next time name it .GameLift.x instead?
	namespace GameSessionsAPI
	{
	//"on GameSession API" kind of weird to me, it is "on some fleet" lol, but anyway.
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ListFleets, "DedicatedServersTags.GameSessionsAPI.ListFleets", "ListFleets resource on GameSession API  - GET");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(FindOrCreateGameSession, "DedicatedServersTags.GameSessionsAPI.FindOrCreateGameSession", "Find an ACTIVE game session or create one if it doesn't exist on GameSession API - POST");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(CreatePlayerSession, "DedicatedServersTags.GameSessionsAPI.CreatePlayerSession", "CreatePlayerSession on GameSession API - POST");
	}

	//next time name it .Cognito.x instead?
	namespace PortalAPI
	{
	//next time name those lambda "SignIn", "SignUp", "ConfirmSignUp" without the prefix Cognito for consistency lol? yes! surely!
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(SignIn, "DedicatedServersTags.PortalAPI.SignIn", "retrieve ID, Access, Refresh tokens from Cognito's User Pool - POST"); //having body content must POST+
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(SignUp, "DedicatedServersTags.PortalAPI.SignUp", "create new user on Cognito's User Pool - POST"); //stephen in Portal API
		//the state is "unconfirmed" to "confirmed" is just "an update" (the field/data already created since SignUp process!)
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ConfirmSignUp, "DedicatedServersTags.PortalAPI.ConfirmSignUp", "Confirm a new user on Cognito's User Pool - PUT"); //stephen in Portal API

		UE_DEFINE_GAMEPLAY_TAG_COMMENT(SignOut, "DedicatedServersTags.PortalAPI.SignOut", "Sign out - POST"); //stephen in Portal API
	}
}
