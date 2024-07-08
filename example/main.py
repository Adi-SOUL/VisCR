from connect_to_sim import default_one_step, Simulator


simularor = Simulator()

simularor.connect()
simularor.set_one_step_func(default_one_step)
simularor.run()
