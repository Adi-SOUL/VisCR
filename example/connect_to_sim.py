import os
from ctypes import c_double, c_int
from typing import Callable

import numpy as np
from _ctypes import POINTER

from ctypes import CDLL


class CanNotReceiveBufferError(Exception):
	def __init__(self, *args, **kwargs):
		super().__init__(self, *args, **kwargs)


class CanNotSendBufferError(Exception):
	def __init__(self, *args, **kwargs):
		super().__init__(self, *args, **kwargs)


class CanNotInitializeSocketError(Exception):
	def __init__(self, *args, **kwargs):
		super().__init__(self, *args, **kwargs)


class WrongSocketVersionError(Exception):
	def __init__(self, *args, **kwargs):
		super().__init__(self, *args, **kwargs)


class CanNotConnectToServerError(Exception):
	def __init__(self, *args, **kwargs):
		super().__init__(self, *args, **kwargs)


def rotate_4x4(axis: str, angle: float) -> np.ndarray:
	if axis not in ['x', 'y', 'z']:
		raise NotWithinTheOptionalRangeError("axis must be 'x', 'y', 'z'")
	cos_angle = np.cos(angle)
	sin_angle = np.sin(angle)
	if axis == "x":
		return np.array([
				[1, 0, 0, 0],
				[0, cos_angle, -sin_angle, 0],
				[0, sin_angle, cos_angle, 0],
				[0, 0, 0, 1]
		])
	elif axis == "y":
		return np.array([
				[cos_angle, -sin_angle, 0, 0],
				[sin_angle, cos_angle, 0, 0],
				[0, 0, 1, 0],
				[0, 0, 0, 1]
		])
	elif axis == "z":
		return np.array([
				[cos_angle, 0, sin_angle, 0],
				[0, 1, 0, 0],
				[-sin_angle, 0, cos_angle, 0],
				[0, 0, 0, 1]
		])


def displacement_4x4(x: float, y: float, z: float) -> np.ndarray:
	return np.array([
			[1, 0, 0, -x],
			[0, 1, 0, z],
			[0, 0, 1, y],
			[0, 0, 0, 1]
	])


def default_one_step(step: int) -> list[np.ndarray]:
	__num__: int = 100
	__length__: float = 200.
	theta: float = np.pi / 3
	phi: float = (2 * np.pi) * (step / 1000)

	one_step_result: list[np.ndarray] = []
	for delta in range(__num__):
		d_theta: float  = theta * (delta + 1) / __num__
		d_length: float = __length__ * (delta + 1) / __num__
		R_1: np.ndarray = rotate_4x4('z', phi)
		R_2: np.ndarray = rotate_4x4('y', d_theta)
		R_3: np.ndarray = rotate_4x4('z', -phi)

		x, y, z = d_length*np.cos(phi)/d_theta*(1-np.cos(d_theta)), d_length*np.sin(phi)/d_theta*(1-np.cos(d_theta)), d_length*np.sin(d_theta)/d_theta
		D: np.ndarray = displacement_4x4(x, y, z)

		T: np.ndarray = np.matmul(D, np.matmul(np.matmul(R_1, R_2), R_3))
		one_step_result.append(T)

	return one_step_result


# noinspection PyProtectedMember
class Simulator:
	def __init__(self):
		self.dll = CDLL(r'.\dll\Sim.dll')
		self.dll._connect.restype = c_int
		self.dll.send_one_step.restype = c_int
		self.dll.send_one_step.argtypes = (c_int, POINTER(c_double))
		self.sim_one_step_func: Callable[[int], list[np.ndarray]] = ...

	def set_one_step_func(self, func: Callable[[int], list[np.ndarray]]):
		self.sim_one_step_func = func

	def connect(self):
		res = self.dll._connect()
		if res == 1:
			raise CanNotInitializeSocketError
		elif res == 2:
			raise WrongSocketVersionError
		elif res == 3:
			raise CanNotConnectToServerError
		else:
			print("Succeed connect to Server")

	def run(self):
		step = 1
		while True:
			results = self.sim_one_step_func(step)
			to_sim = []
			for res in results:
				to_sim += res.reshape(1, -1).tolist()[0]
			to_sim = np.array(to_sim)
			to_sim = to_sim.ctypes.data_as(POINTER(c_double))
			send_res = self.dll.send_one_step(len(results), to_sim)
			if send_res == 1:
				raise CanNotReceiveBufferError
			elif send_res == 2:
				raise CanNotSendBufferError
			step += 1
