#pragma once
#include "Define.h"
#include "EngineBaseTypes.h"
#include "EngineTypes.h"
#include "Level.h"
#include "Container/Set.h"
#include "UObject/ObjectFactory.h"
#include "UObject/ObjectMacros.h"
#include "Runtime/Engine/Classes/Components/PostProcessing/FogComponent.h"
#include "Runtime/Engine/Classes/Components/SkySphereComponent.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
class FObjectFactory;
class AActor;
class UObject;
class UGizmoArrowComponent;
class UCameraComponent;
class AEditorPlayer;
class USceneComponent;
class UTransformGizmo;

class UWorld final : public UObject
{
    DECLARE_CLASS(UWorld, UObject)

public:
    UWorld() = default;
    UWorld(const UWorld& Other);
    ;
    void InitWorld();
    void CreateBaseObject();
    void ReleaseBaseObject();
    void Tick(ELevelTick tickType, float deltaSeconds);
    void Release();
    void ReloadScene(const FString& FileName);
    void ClearScene();
    void SaveScene(const FString& FileName);
    virtual UObject* Duplicate() const override;
    virtual void DuplicateSubObjects(const UObject* SourceObj) override;
    virtual void PostDuplicate() override;
    /**
     * World에 Actor를 Spawn합니다.
     * @tparam T AActor를 상속받은 클래스
     * @return Spawn된 Actor의 포인터
     */
    template <typename T>
        requires std::derived_from<T, AActor>
    T* SpawnActor();

    /** World에 존재하는 Actor를 제거합니다. */
    bool DestroyActor(AActor* ThisActor);

private:
    const FString defaultMapName = "Default";
    ULevel* Level;
    /** World에서 관리되는 모든 Actor의 목록 */
    /** Actor가 Spawn되었고, 아직 BeginPlay가 호출되지 않은 Actor들 */
    AActor* SelectedActor = nullptr;
    UActorComponent* SelectedComponent = nullptr;
    USceneComponent* pickingGizmo = nullptr;
    AEditorPlayer* EditorPlayer = nullptr;
    
public:
    EWorldType::Type WorldType = EWorldType::None;
    const TSet<AActor*>& GetActors() const { return Level->GetActors(); }
    ULevel* GetLevel() const { return Level; }
    UTransformGizmo* LocalGizmo = nullptr;
    AEditorPlayer* GetEditorPlayer() const { return EditorPlayer; }
    // EditorManager 같은데로 보내기
    AActor* GetSelectedActor() const { return SelectedActor; }
    void SetPickedActor(AActor* InActor)
    {
        if (Cast<USkySphereComponent>(InActor->GetRootComponent())) {
            SelectedActor = NULL;
            SelectedComponent = NULL;
            return;
        }
        SelectedActor = InActor;
    }

    UActorComponent* GetSelectedComponent() const { return SelectedComponent; }
    void SetPickedComponent(UActorComponent* InComponent)
    {
        SelectedComponent = InComponent;
    }

    USceneComponent* GetPickingGizmo() const { return pickingGizmo; }
    void SetPickingGizmo(UObject* Object);

    // 임시
    bool IsPIEWorld() const;
    void BeginPlay();
    static UWorld* DuplicateWorldForPIE(UWorld* world);
};
// UWorld* GWorld = nullptr;

template <typename T>
    requires std::derived_from<T, AActor>
T* UWorld::SpawnActor()
{
    T* Actor = FObjectFactory::ConstructObject<T>();
    // TODO: 일단 AddComponent에서 Component마다 초기화
    // 추후에 RegisterComponent() 만들어지면 주석 해제
    // Actor->InitializeComponents();
    
    Level->GetActors().Add(Actor);
    Level->PendingBeginPlayActors.Add(Actor);
    return Actor;
}
