# EngineConstruct
Engineering Thesis

# Usage Notes
While adding new Engine Models to project, make sure to turn off [Simulate Physics] from all the component object is made of (especially root component).
It will prevent model from acting wierd, also you don't really want to move the root.

Trying to reattach, previously detached part, keep in mind that you can do it only for parts which parent is attached (root object doesn't have parent, but as it's base and it cannot be moved, reattaching works always for any part that has root as parent).