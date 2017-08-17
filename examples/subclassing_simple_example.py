from unreal_engine.classes import Character

class Hero(Character):

    def __init__(self):
        self.CapsuleComponent.CapsuleRadius = 117
        self.CapsuleComponent.CapsuleHalfHeight = 200

        # BodyInstance is a USTRUCT (they are always passed as value)
        body_instance = self.CapsuleComponent.BodyInstance
        body_instance.CollisionProfileName = 'OverlapAll'
        self.CapsuleComponent.BodyInstance = body_instance

        self.CharacterMovement.GravityScale = 0.0

    def OnActorBeginOverlap(self, other_actor):
        print('overlapping with {0}'.format(other_actor))
