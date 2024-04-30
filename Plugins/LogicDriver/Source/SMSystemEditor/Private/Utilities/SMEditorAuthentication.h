// Copyright Recursoft LLC 2019-2023. All Rights Reserved.

#pragma once

#include "Configuration/SMEditorSettings.h"
#include "SMBlueprintEditorUtils.h"

#include "Misc/SMAuthenticator.h"

namespace LD::EditorAuth
{
	enum LDAuthStatus
	{
		Never = 0,
		Succeeded = 1,
		Failed = 2,
	};

	/** Retrieve the last recorded authentication state. */
	inline LDAuthStatus GetUserMarketplaceAuthenticationStatus()
	{
		return static_cast<LDAuthStatus>(FSMBlueprintEditorUtils::GetEditorSettings()->UserMarketplaceAuthenticationState);
	}
	
	/** Perform an entitlement check and record the result to the user's editor settings. */
	inline void AuthenticateAndRecordResult(const FSimpleDelegate& InOnAuthenticatedDelegate,
		IPluginWardenModule::EUnauthorizedErrorHandling InUnauthorizedErrorHandling =
		IPluginWardenModule::EUnauthorizedErrorHandling::ShowMessageOpenStore,
		bool bForceAuthenticate = false)
	{
#if LOGICDRIVER_IS_MARKETPLACE_BUILD
		FSMBlueprintEditorUtils::GetMutableEditorSettings()->UserMarketplaceAuthenticationState = LDAuthStatus::Failed;
		FSMBlueprintEditorUtils::GetMutableEditorSettings()->SaveConfig();
#endif
		
		FSMAuthenticator::Get().Authenticate(FSimpleDelegate::CreateLambda([InOnAuthenticatedDelegate]()
		{
#if LOGICDRIVER_IS_MARKETPLACE_BUILD
			FSMBlueprintEditorUtils::GetMutableEditorSettings()->UserMarketplaceAuthenticationState = LDAuthStatus::Succeeded;
			FSMBlueprintEditorUtils::GetMutableEditorSettings()->SaveConfig();
#endif
			InOnAuthenticatedDelegate.ExecuteIfBound();
		}),
		InUnauthorizedErrorHandling, bForceAuthenticate);
	}

	/** Perform an entitlement check and record the result to the user's editor settings. */
	inline void AuthenticateAndRecordResultSilently()
	{
		AuthenticateAndRecordResult(FSimpleDelegate(), IPluginWardenModule::EUnauthorizedErrorHandling::Silent);
	}

	/** Perform an entitlement check and record the result to the user's editor settings, only if this hasn't been done before. */
	inline void AuthenticateAndRecordResultOnceSilently()
	{
		if (GetUserMarketplaceAuthenticationStatus() == LD::EditorAuth::Never)
		{
			AuthenticateAndRecordResultSilently();
		}
	}
}
