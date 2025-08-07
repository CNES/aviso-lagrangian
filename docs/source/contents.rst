Lagrangian Coherent Structures and Finite-Time Lyapunov Exponents
=================================================================

The transport of a tracer in a fluid is closely related to emergent patterns
that are commonly referred to as coherent structures or *Lagrangian Coherent
Structures* (LCS) when they are defined using fluid trajectories. For time-
independent dynamic systems, they correspond to stable and unstable manifolds
of hyperbolic trajectories [1]_. Coherent structures delimit regions of
whirls, stretching, or contraction of tracer [2]_: contraction is observed
along stable manifolds whereas unstable manifolds correspond to divergent
directions along which the tracer is stretched. Stable and unstable manifolds
are material curves that act as transport barriers and that exhibits locally
the strongest attraction, repulsion or shearing in the flow over a finite-time
interval [3]_. LCSs are usually identified in a practicall manner as
maximizing ridges of Finite-Time Lyapunov Exponents (FTLE) field [4]_, [5]_,
[6]_, [7]_, [8]_.

FTLE is defined as the largest eigenvalue of the Cauchy-Green strain tensor of
the flow map (see below for more details). The corresponding eigenvector is
called Finite-Time Lyapunov Vector. FTLE and FSLE are widely used to
characterise transport and mixing processes in oceanographic flows where the
velocity field is only known as a finite data set [9]_, [10]_, [11]_, [12]_,
[13]_, [14]_, [15]_, [16]_, [17]_, [18]_, [19]_, [20]_, [21]_. Recent work
also demonstrate the potential use of FTLE in tracer image assimilation in
geophysical models [22]_, [23]_.

.. seealso::

	[24]_ which reviews the use of FTLE for detecting LCSs in other contexts.

FTLE and FSLE: definition and properties
########################################

FTLE is a scalar local notion that represents the rate of separation of
initially neighboring particules over a finite-time window :math:`[t, t+T]`.
FTLE at a point :math:`x` is defined as the growth factor of the norm of the
perturbation delta started around :math:`x` at time :math:`t` and advected by
the flow after an advection time :math:`T`. Maximal stretching occurs when
delta is aligned with the eigenvector associated to the maximum eigenvalue
:math:`\lambda_{max}` of the Cauchy-Green strain tensor: :math:`Î” = M^T M`
where :math:`M : x(t) \mapsto x(t+T)` is the flow map of the advection system.
This eigenvector is referred to as the *forward* Finite-Time Lyapunov Vector.
The *forward* FTLE at the point :math:`x` is then defined as:
:math:`FTLE(x) = \frac{log(Î»max)}{2T}`.

FSLE are FTLE where T is not fixed a-priori but determined so that the
particule separate from a given distance. FSLE at a point x and for a final
separation distance d is defined as: :math:`FSLE(x, d) =
\frac{log(Î»max(d))}{2T(d)}`. Seeding a domain with particules initially
located on a grid leads to the computation of a discretized FSLE and FSLV
fields.

*Backward* FSLE-Vs are similarly defined, with the time direction being
inverted in the advection equation resolution. Ridges of BFTLE fields
approximate attracting LCSs [25]_.

Backward FTLE fields show contours that correspond reasonably well to the main
structures such as filaments, fronts and spirals that appear in geophysical
and bio-geochemical tracer fields [10]_, [14]_, [15]_, [26]_. Also,
orientation of the gradient of a *passive* tracer converges to that of
backward FTLVs freely decaying 2D turbulence flow [27]_ but this behaviour has
been observed for realistic oceanic flows and tracers [20]_.

Furthermore [11]_, [28]_ showed, using real data, that properties of FSLE and
FTLE remain valid with a mesoscale advection, i.e. when the resolution of the
velocity field --- from which FTLE-V are computed --- is much lower than the
resolution of the observed tracer field [20]_ ,[21]_.

References
##########

.. [1] Wiggins, S. 1992. Chaotic transport in dynamical systems. Vol. 2 of
	*Interdisciplinary Applied Mathematics*. Springer-Verlag, Berlin.

.. [2] Ottino, J. 1989. The kinematics of mixing: Stretching, chaos, and
	transport. Cambridge University Press, Cambridge.

.. [3] Haller, G. and Yuan, G. 2000. Lagrangian coherent structures and mixing
	in two-dimensional turbulence. *Physica D: Nonlinear Phenomena* **147**
	(3-4): 352--370.

.. [4] Haller, G. 2001. Distinguished material surfaces and coherent structures
	in three-dimensional fluid flows. *Physica D* **149** (4): 248 -- 277.

.. [5] Haller, G. 2001. Lagrangian structures and the rate of strain in a
	partition of two-dimensional turbulence. *Physics of Fluids* **13**
	(11): 3365 --3385.

.. [6] Haller, G. 2002. Lagrangian coherent structures from approximate
	velocity data. *Physics of Fluids* **14** (6): 1851--1861.

.. [7] Shadden, S.~C., Lekien, F. and Marsden, J. E. 2005. Definition and
	properties of lagrangian coherent structures from finite-time lyapunov
	exponents in two-dimensional aperiodic flows. *Physica D: Nonlinear Phenomena*
	**212** (3-4): 271--304.

.. [8] Mathur, M., Haller, G., Peacock, T., Ruppert-Felsot, J. E. and Swinney, H.
	L. 2007. Uncovering the lagrangian skeleton of turbulence. *Phys. Rev. Lett.*
	**98** (14): 144502

.. [9] Beron-Vera, F. J., Brown, M.~G., Olascoaga, M. J., Rypina, I. I., KoÃ§ak,
	H. and Udovydchenkov, I. A. 2008. Zonal jets as transport barriers in
	planetary atmospheres. *Journal of the Atmospheric Sciences* **65** (10): 3316
	--3326

.. [10] Beron-Vera, F. J. and Olascoaga, M. J. 2009. An assessment of the
	importance of chaotic stirring and turbulent mixing on the west florida shelf.
	*Journal of Physical Oceanography* **39** (7): 1743--1755.

.. [11] Beron-Vera, F. J., Olascoaga, M. J. and Goni, G. J. 2010. Surface ocean
	mixing inferred from different multisatellite altimetry measurements. *Journal
	of Physical Oceanography* **40** (11): 2466--2480.

.. [12] Lekien, F., Coulliette, C., Mariano, A. J., Ryan, E. H., Shay, L. K.,
	Haller, G. and Marsden, J. 2005. Pollution release tied to invariant
	manifolds: A case study for the coast of florida. *Physica D: Nonlinear
	Phenomena* **210** (1-2): 1--20.

.. [13] Olascoaga, M. J. 2010. Isolation on the west florida shelf with
	implications for red tides and pollutant dispersal in the gulf of mexico.
	*Nonlinear Processes in Geophysics* **17** (6): 685--696.

.. [14] Olascoaga, M. J., Beron-Vera, F. J., Brand, L. E. and Kocak, H. 2008.
	Tracing the early development of harmful algal blooms on the west florida
	shelf with the aid of lagrangian coherent structures. *J. Geophys. Res.*
	**113** (C12014).

.. [15] Shadden, S.~C., Lekien, F., Paduan, J. D., Chavez, F. P. and Marsden, J.
	E. 2009. The correlation between surface drifters and coherent structures
	based on high-frequency radar data in monterey bay. *Deep Sea Research Part
	II: Topical Studies in Oceanography* **56** (3-5): 161--172.

.. [16] Aurell, E., Boffetta, G., Crisanti, A., Paladin, G. and Vulpiani, A.,
	1997. Predictability in the large: an extension of the concept of lyapunov
	exponent. *Journal of Physics A: Mathematical and General* **30** (1):1--26

.. [17] Artale, V., Boffetta, G., Celani, A., Cencini, M. and Vulpiani, A., 1997.
	Dispersion of passive tracers in closed basins: Beyond the diffusion
	coefficient. *Physics of Fluids* **9** (11):3162--3171.

.. [18] d'Ovidio, F., Fernandez, V., Hernandez-Garcia, E. and Lopez, C. 2004.
	Mixing structures in the mediterranean sea from finite-size lyapunov
	exponents. *Geophysical Research Letters* **31** (L17203).

.. [19] d'Ovidio, F., Isern-Fontanet, J., Lopez, C., Hernandez-Garcia, E. and
	Garcia-Ladona, E. 2009. Comparison between eulerian diagnostics and finite-
	size lyapunov exponents computed from altimetry in the algerian basin. *Deep
	Sea Research Part I: Oceanographic Research Papers* **51** (1): 15--31.

.. [20] d'Ovidio, F., Taillandier, V., Taupier-Letage, I. and Mortier, L. 2009.
	Lagrangian validation of the mediterranean mean dynamic topography by
	extraction of tracer frontal structures. *Mercator Ocean Quarterly Newsletter*
	**32**: 24--32.

.. [21] Lehahn, Y., d'Ovidio, F., LÃ©vy, M. and Heifetz, E. 2007. Stirring of the
	northeast atlantic spring bloom: A lagrangian analysis based on multisatellite
	data. *J. Geophys. Res.* **112** (C08005).

.. [22] O. Titaud, J.-M. Brankart, and J. Verron. 2011. On the use of finite-time
	Lyapunov exponents and vectors for direct assimilation of tracer images into
	ocean models. *Tellus A*, **63** (5):1038-1051.

.. [23] L. Gaultier, J. Verron, J.-M. Brankart, O. Titaud, and P. Brasseur. 2012.
	On the inversion of submesoscale tracer fields to estimate the surface ocean
	circulation. *Journal of Marine Systems* **126**: 33--42.

.. [24] Peacock, T. and Dabiri, J. 2010. Introduction to focus issue: Lagrangian
	coherent structures. * Chaos* **20** (1): 017501.

.. [25] Haller, G. 2011. A variational theory of hyperbolic lagrangian coherent
	structures. *Physica D: Nonlinear Phenomena* **240** (7): 574 -- 598.

.. [26] Olascoaga, M. J., Rypina, I. I., Brown, M. G., Beron-Vera, F. J., Kocak,
	H., Brand, L.~E., Halliwell, G.~R. and Shay, L.~K. 2006. Persistent transport
	barrier on the west florida shelf. *Geophys. Res. Lett.* **33** (L22603).

.. [27] Lapeyre, G. 2002. Characterization of finite-time lyapunov exponents and
	vectors in two-dimensional turbulence. *Chaos* **12** (3): 688--698.

.. [28] Beron-Vera, F. J. Mixing by low- and high-resolution surface geostrophic
	currents. 2010. *J. Geophys. Res*. **115** (C10027)
