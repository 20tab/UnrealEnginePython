import unreal_engine as ue
from unreal_engine.classes import ParticleSystem

for i in range(0, 10):
	# create a particle system
	particle_system = ParticleSystem()
	# give it a name
	particle_system.set_name('ParticleSystem{0}'.format(i))
	# save it into a package
	particle_system.save_package('/Game/FunnyParticles/ParticlePackage{0}'.format(i))


# now ready for mass renaming !!!

# each particle system will ends in a different folder (bur all of the particle system will have the same name)

for i in range(0, 10):
	ue.rename_asset('/Game/FunnyParticles/ParticlePackage{0}.ParticleSystem{0}'.format(i), '/Game/MovedParticles/{0}'.format(i), 'RenamedParticle')
