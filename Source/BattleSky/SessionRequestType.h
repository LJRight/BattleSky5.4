#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"

/**
 * UI → LANSessionSubsystem 으로 전달되는
 * 단일 세션 요청 이벤트 구조체
 */
struct FSessionRequest
{
	/** 요청 타입 */
	enum class EType : uint8
	{
		Create,
		Find,
		Join,
		Leave
	};

	EType Type = EType::Find;

	/** Join 요청일 때만 사용 */
	const FOnlineSessionSearchResult* TargetSession = nullptr;

public:
	FSessionRequest() = default;

	explicit FSessionRequest(EType InType)
		: Type(InType)
	{
	}

	FSessionRequest(const FOnlineSessionSearchResult& InTargetSession)
		: Type(EType::Join)
		, TargetSession(&InTargetSession)
	{
	}
};
